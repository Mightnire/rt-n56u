#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <stdarg.h>
#include <sys/wait.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <httpd.h>

#include <shutils.h>

#define FW_CREATE	0
#define FW_APPEND	1
#define FW_NEWLINE	2

// for backup =========================================================

int f_write(const char *path, const void *buffer, int len, unsigned flags, unsigned cmode)
{
	static const char nl = '\n';
	int f;
	int r = -1;
	mode_t m;

	m = umask(0);
	if (cmode == 0) cmode = 0666;
	if ((f = open(path, (flags & FW_APPEND) ? (O_WRONLY|O_CREAT|O_APPEND) : (O_WRONLY|O_CREAT|O_TRUNC), cmode)) >= 0) {
		if ((buffer == NULL) || ((r = write(f, buffer, len)) == len)) {
			if (flags & FW_NEWLINE) {
				if (write(f, &nl, 1) == 1) ++r;
			}
		}
		close(f);
	}
	umask(m);
	return r;
}

// for bandwidth =========================================================

int f_exists(const char *path)	// note: anything but a directory
{
	struct stat st;
	return (stat(path, &st) == 0) && (!S_ISDIR(st.st_mode));
}

static int _f_wait_exists(const char *name, int max, int invert)
{
	max *= 40;
	while (max-- > 0) {
		if (f_exists(name) ^ invert)
			return 1;
		usleep(25000);
	}
	return 0;
}

int f_wait_exists(const char *name, int max)
{
	return _f_wait_exists(name, max, 0);
}

int f_read(const char *path, void *buffer, int max)
{
	int f;
	int n;

	if ((f = open(path, O_RDONLY)) < 0) return -1;
	n = read(f, buffer, max);
	close(f);
	return n;
}

int f_read_string(const char *path, char *buffer, int max)
{
	if (max <= 0) return -1;
	int n = f_read(path, buffer, max - 1);
	buffer[(n > 0) ? n : 0] = 0;
	return n;
}

size_t strlcpy(char *d, const char *s, size_t bufsize)
{
	size_t len = strlen(s);
	size_t ret = len;
	if (bufsize <= 0) return 0;
	if (len >= bufsize) len = bufsize-1;
	memcpy(d, s, len);
	d[len] = 0;
	return ret;
}

/**
 * hex char to int
 * @author 荒野无灯
 * @date 2016-08-01 12:30
 * @param hex
 * @return int
 */
static int hex_char_to_int(const uint8_t *hex)
{
	int val;
	if(*hex >= '0' && *hex <='9') {
		val = (*hex - '0') * 16;
	} else {
		val = (*hex - 'A' + 10) * 16;
	}

	if(*(hex+1) >= '0' && *(hex+1) <='9') {
		val += (*(hex+1) - '0');
	} else {
		val += (*(hex+1) - 'A' +10);
	}

	return val;
}


static int hex_nibble(uint8_t c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	return -1;
}

/* Trim incomplete trailing UTF-8 multibyte sequence to avoid JS URIError. */
static size_t trim_incomplete_utf8(uint8_t *buf, size_t len)
{
	size_t i = 0;

	while (i < len) {
		uint8_t c = buf[i];
		size_t seq_len = 1;

		if (c < 0x80)
			seq_len = 1;
		else if ((c & 0xE0) == 0xC0)
			seq_len = 2;
		else if ((c & 0xF0) == 0xE0)
			seq_len = 3;
		else if ((c & 0xF8) == 0xF0)
			seq_len = 4;
		else
			break;

		if (i + seq_len > len)
			break;

		i += seq_len;
	}
	buf[i] = '\0';
	return i;
}

void char_to_ascii(char *output, uint8_t *input)
{
	int i;
	char tmp[10];
	char *ptr;
	int input_len;

	ptr = output;
	input_len = strlen((const char *)input);

	for (i = 0; i < input_len; i++) {
		if ((input[i] >= '0' && input[i] <= '9')
		   || (input[i] >= 'A' && input[i] <= 'Z')
		   || (input[i] >= 'a' && input[i] <= 'z')
		   || input[i] == '!' || input[i] == '*'
		   || input[i] == '(' || input[i] == ')'
		   || input[i] == '_' || input[i] == '-'
		   || input[i] == '\'' || input[i] == '.') {
			*ptr = input[i];
			ptr++;
		} else {
			sprintf(tmp, "%%%.02X", input[i]);
			strcpy(ptr, tmp);
			ptr += 3;
		}
	}

	*ptr = '\0';
}

/* Decode driver "0x..." hex-encoded site-survey SSIDs safely before URL encoding. */
void scan_ssid_to_ascii(char *output, uint8_t *input)
{
	size_t input_len = strlen((const char *)input);
	uint8_t decoded[64];
	size_t i, out_len = 0;

	if (input_len >= 4 && (input_len % 2) == 0 && input[0] == '0' && input[1] == 'x') {
		for (i = 2; i < input_len && out_len < sizeof(decoded) - 1; i += 2) {
			int hi = hex_nibble(input[i]);
			int lo = hex_nibble(input[i + 1]);
			if (hi < 0 || lo < 0) {
				out_len = 0;
				break;
			}
			decoded[out_len++] = (uint8_t)((hi << 4) | lo);
		}
		if (out_len > 0) {
			decoded[out_len] = '\0';
			trim_incomplete_utf8(decoded, out_len);
			char_to_ascii(output, decoded);
			return;
		}
	}

	char_to_ascii(output, input);
}

int do_f(const char *path, webs_t wp)
{
	FILE *fp;
	char buf[1024];
	int ret = 0;

	fp = fopen(path, "r");
	if (fp) {
		while (fgets(buf, sizeof(buf), fp))
			ret += fputs(buf, wp);
		fclose(fp);
	} else {
		ret += fputs("", wp);
	}

	fflush(wp);

	return ret;
}

