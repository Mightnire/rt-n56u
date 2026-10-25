更新日志

2026/10/25：
- 重构并上线现代化 GitHub Actions 自动化编译工作流（.github/workflows/build-firmware.yml）：
  - 彻底淘汰已弃用的老旧 CI.yml 与 actions/upload-artifact@v2，全面迁移至 Ubuntu 22.04 LTS 运行环境与 actions/checkout@v4、upload-artifact@v4、action-gh-release@v2；
  - 修复云端构建工作目录路径不匹配缺陷：通过 sudo ln -sfn "$GITHUB_WORKSPACE" /opt/rt-n56u 将 GitHub Actions 默认工作目录软链接至 Padavan 源码与工具链强依赖的 /opt/rt-n56u 绝对路径；
  - 修复 tools/mksquashfs_xz 跨目录编译失败：移除仓库残留的已配置标记 config_xz_done，并在 Makefile 中加入工作路径校验，若检测到构建目录变动自动重新运行 configure_xz；
  - 支持交互式手动调度（workflow_dispatch）：提供机型下拉选择菜单（BW050-A、MI-R3G、MI-R3P-spi、PR2000、PSG1218、一键全选或自定义机型名），支持勾选一键直接发布至 GitHub Releases；
  - 支持 Git Tag (v*) 自动化触发与持续交付：打标签推送自动并行构建四大核心机型固件并自动计算校验和发布。
- 更新根证书库（CA Certificates）并全面移除不信任的印度 eMudhra (emSign) CA 证书：
  - 同步并引入 Mozilla 2026 年 10 月最新发布的根证书信任库，全面更新陈旧失效的根证书；
  - 彻底剥离并移除涉及严重弱私钥缺陷（“Key of 4”漏洞、已被 Google/Mozilla/Apple 紧急封杀）的印度 CA 机构 eMudhra / emSign 旗下全部根证书（emSign Root CA - G1/C1、emSign ECC Root CA - G3/C3）；
  - 新增 CONFIG_FIRMWARE_INCLUDE_CA_CERTIFICATES 编译选项：允许在模板配置中按需开启（大容量机型默认开启，8MB/nano 机型可裁减省出空间），开启时自动将 CA 证书信任链（/etc_ro/ca-certificates.crt）打包进 ROMFS，在 dev_init.sh 启动时自动软链至 OpenSSL 标准路径 /etc/ssl/cert.pem 与 Linux 标准路径 /etc/ssl/certs/ca-certificates.crt（支持 /etc/storage 优先覆写）；
  - 优化 libcurl 构建配置，将默认证书路径绑定至 /etc/ssl/cert.pem，并在用户环境中预设 SSL_CERT_FILE / CURL_CA_BUNDLE，彻底根治 curl、inadyn、aria2 等工具在 HTTPS 下的 SSL certificate problem 校验失败故障。
- 重构与更新主仓库 README.md：全面汇总项目关键优化特性（OpenSSL 1.1.1w、Transmission 3.07.1、Aria2 容错增强、智能负载心跳律动、多语言翻译补齐），整理全机型支持矩阵（补全 NETGEAR PR2000、MI-R3P-spi 等新型号），并更新源码克隆地址与环境构建说明。
- 优化 Aria2 初始配置与断点续传恢复能力：在 user/aria2/aria.sh 初始配置文件模板中新增 allow-piece-length-change=true，允许任务分片大小在重定向、连接数调整或控制文件不一致时平滑恢复，彻底消除 ErrorCode=27（分片大小不匹配）引发的任务中断故障。
- 升级 Transmission 至现代维护分支 3.07.1（Mightnire/transmission-og）：
  - 剥离上游分支引入的非标 -og 命令与安装目标前后缀，完全还原为原生标准命令集（transmission-daemon、transmission-remote 等），实现与 Padavan 守护进程及 WebUI 脚本无缝兼容；
  - 随包内联完整第三方库源码（dht, miniupnpc, libutp, libb64, libnatpmp, libevent），摆脱 Git 子模组与外部网络依赖，构建期自给自足；
  - 编写并应用 transmission-3.07.1.patch，保留 Padavan 专属网络微调（默认关闭 RPC 外网白名单、默认关闭路由器本地冗余 UPnP 映射、UDP 缓冲区减半优化以节省路由器内存）；
  - 全面支持 64 位 LFS（大文件偏移量系统调用），彻底解决单任务下载 >2GB 导致写入溢出中断问题。
- 修复 Transmission 无法下载大于 2GB 单文件缺陷（启用 LFS 64位大文件支持）：在 trunk/user/transmission/Makefile 与 cross-mipsel-linux.cmake 中显式定义 -D_FILE_OFFSET_BITS=64 并将 CMake 目标工具链文件调整为绝对路径，确保 pwrite/lseek/stat 等系统调用全面升级为 pwrite64/lseek64/stat64，彻底根治 32 位 MIPS 平台下单文件超过 2GB 偏移量溢出沦为负数导致的 EINVAL (Invalid argument) 写入中断。

2026/10/24：
- 修复 MI-R3P 系列编译失败（undefined reference to `dl_update_power_led`）：此前 CONFIG_FIRMWARE_INCLUDE_POWER_LED_HEARTBEAT 编译开关仅将 dl_update_power_led() 的定义包进 APP_PWR_HEARTBEAT，而 dl_on_timer()/dl_update_leds() 的调用点仍只判断 BOARD_GPIO_LED_POWER，导致所有定义了电源灯但未开启该特性的机型（MI-R3P-spi、MI-R3P、K2P、DIR-878 等约 50 款）在 user/rc 链接阶段报错；现将函数定义条件放宽为 BOARD_GPIO_LED_POWER，仅把 CPU 占用率采样与负载心跳/呼吸模式保留在 APP_PWR_HEARTBEAT 内，未启用心跳的机型回退到“关闭/常亮”逻辑；
- MI-R3P、MI-R3P-spi、MI-R3P-breed 启用 CONFIG_FIRMWARE_INCLUDE_POWER_LED_HEARTBEAT=y，电源灯支持按 CPU 负载的心跳/呼吸律动；
- 二次配置分区内核选项 CONFIG_MTD_CONFIG2_PART 除 PR2000 外全部在板级内核配置中显式写入 # CONFIG_MTD_CONFIG2_PART is not set（共 58 个配置文件，含 5.0 与 breed 变体），彻底消除其它机型构建时出现的 (NEW) 交互询问。
- MI-R3G 与 MI-R3P-spi 启用 CONFIG_FIRMWARE_INCLUDE_6RELAYD=y（支持 3 层 IPv6 动态中继）；
- MI-R3G 启用 CONFIG_FIRMWARE_INCLUDE_POWER_LED_HEARTBEAT=y，前面板蓝色电源灯支持基于 CPU 负载与系统平均负载的智能生命律动；
- 梳理澄清 MI-R3G 无线驱动架构，确认 MT76X3_AP 对应 MT7603E 2.4GHz 官方驱动模块，破除历史误判禁令。

2026/10/18：
- 新增 NETGEAR PR2000 便携式无线旅行路由器（MT7620N）全套板级适配与专属固件移植：
  - 硬件逆向与引脚映射：
    - 复位按键：BOARD_GPIO_BTN_RESET 映射至物理 GPIO 1（低电平有效）；
    - 指示灯体系：BOARD_GPIO_LED_POWER 映射至 GPIO 72（主状态灯，支持生命律动心跳），BOARD_GPIO_LED_USB 映射至 GPIO 0，BOARD_GPIO_LED_LAN 映射至 GPIO 44；
    - 交换机端口重排：黄色复合口定义为 WAN（Port 4），白色网口定义为 LAN1（Port 0），解决双口旅行路由网口映射与 duplicate case 冲突；
  - 核心缺陷修复与性能突破：
    - 修复原厂 EEPROM 0x36 错误置 0x04 导致的 2.4G 假外置 LNA 缺陷（OpenWrt 遗留大坑），修正为 0x00 强制启用 MT7620 内部高灵敏度 LNA，根治无线接收信号衰减至 <1MB/s 与芯片过载高热；
    - 突破非标 Flash 分区冲刷风险：将原厂藏于 0x300000 的射频标定参数迁移并固化至 Breed 标准 64KB Factory 分区（0x40000）；
  - 出厂贴纸凭据自动同步系统：
    - 从原厂 SC_Private_Data (mtd9) 提取真实出厂 SSID、WPA 密钥、WPS PIN 与硬件 MAC，封装进 Factory 扩展保留区（0x0400）；
    - 在 user/rc/rc.c 的 nvram_restore_defaults() 中加入 PR2000 专属恢复钩子，复位或初次启动时自动同步机身背部贴纸参数（SSID: NETGEAR66, PSK: festivesun652, WPA2-PSK AES），出厂体验无缝闭环；
  - 双配置分区（Config / Config2）与机身物理滑动开关硬件联动：
    - 在 16MB 闪存尾部开辟独立的 64KB 第二配置分区 Config2（mtdblock6，0x00FF0000 ~ 0x01000000），将 Storage 分区收缩至 192KB；
    - 改造 Linux 3.4.x 内核 NVRAM 驱动（nvram_linux.c），在 dev_nvram_init() 启动阶段采样机身物理滑动开关（GPIO 20）：低电平（ALT 档位）挂载 Config2，高电平（默认档位）挂载 Config；
    - 系统 syslog 服务启动时通过 /proc/nvram 自动探测当前生效分区并打印日志（active config partition: Config/Config2），终端与 WebUI 一目了然；
    - WebUI 界面徽标与备份文件名联动：WebUI 顶部固件版本后展示彩色模式徽标（Config 浅蓝 / Config2 绿色），导出配置文件自动命名为 Settings_PR2000_Config.CFG 或 Settings_PR2000_Config2.CFG；
    - 出厂主机名智能区分：Config 恢复默认为主机名 PR2000-Wired，Config2 恢复默认为主机名 PR2000-Wireless，贴纸无线凭据保持无缝同步；
  - SPI Flash 稳定读取模式修复与 SquashFS 解压根治：
    - 修复 MT7620 驱动在 48MHz 下执行双线读取（CONFIG_MTD_SPI_READ_DOR）缺少 Dummy Cycles 导致的位流偏移与 SquashFS xz_dec_run error；
    - 对齐 OpenWrt 官方 DTS (m25p,fast-read) 启用 CONFIG_MTD_SPI_READ_FAST=y（带 1 个 Dummy 哑周期字节采样等待，耐受达 104MHz），彻底根治滑动开关拨向 Wireless 天线侧时射频干扰引发的 SQUASHFS 读校验错误；
  - 指示灯体系精准重构与硬件交换机接管彻底根治：
    - 根治 WiFi 指示灯硬件交换机抢占缺陷：分析查明物理 WiFi 指示灯引脚为 GPIO 44（EPHY_LED4_N），开启内核 CONFIG_RALINK_GPIOMODE_EPHY=y 释放 bit 15，彻底解除内部硬件交换机（EPHY Port 4）对该引脚的硬件直驱锁定；
    - 启用 MT76X2 AP 软闪烁驱动：配置 CONFIG_MT76X2_AP_LED_SOFT=y 与 CONFIG_MT76X2_AP_LED_SOFT_GPIO=44，并在 board.h 中将 WiFi 灯重构为 BOARD_GPIO_LED_SW2G（44），实现无线开启常亮、数据传输软闪烁、无线关闭彻底熄灭；
    - 修复 Internet 指示灯（小地球图标）引脚控制：将 BOARD_GPIO_LED_WAN 修正映射至 GPIO 72，并在 rc.c 的 init_gpio_leds_buttons() 中动态将 GPIOMODE bit 13 切至纯 GPIO 模式，恢复完整的 WAN 连接与外网探测状态指示；
    - 确认 PR2000 主电源灯直连供电轨通电即亮，禁用冗余的 CONFIG_FIRMWARE_INCLUDE_POWER_LED_HEARTBEAT 编译选项并移除相关代码分支；
  - 豪华特性与 BW050-A 100% 对齐：集成 SFE 快捷转发加速、POWER 心跳律动、Transmission 3.00 (v0.1.9 WebUI)、Aria2+AriaNg、Samba 3.6、OpenSSH、KMS (vlmcsd) 及全格式存储驱动，16MB SPI Flash 镜像控制在 11.5MB 精巧区间。
- 完整实现三层 IPv6 中继代理架构（Relay 模式，基于 6relayd，编译宏 CONFIG_FIRMWARE_INCLUDE_6RELAYD=y|n）：
  - 引入极简嵌入式服务守护进程 6relayd（纯 C99 编写，仅依赖标准 C 库与 libresolv，ROMFS 体积仅 44KB）；
  - 完美解决无 DHCPv6-PD 前缀委派环境（单 /64 前缀）下的二级路由 IPv6 下发难题：
    - 保持 WAN 和 LAN 纯三层独立路由架构，IPv4 完整吃满 MTK 硬件 NAT (PPE 硬件加速) 与防火墙保护，局域网终端获取同网段原生公网 IPv6 地址；
    - 完整中继 Router Advertisement (RA 路由广播)、DHCPv6 消息中继以及 Neighbor Discovery Protocol (NDP Proxy 邻居发现代理与路由自动学习)；
    - 路由器自身在 WAN 侧拥有独立公网 IPv6 地址与默认路由，自带满血 PMTU 自动发现与 TCPMSS Clamping，从源头杜绝 MTU 1492 大包黑洞；
    - 联动 dnsmasq：在 Relay 模式下自动禁用 dnsmasq 本地 RA 广播（enable-ra）及地址池构造器，杜绝网段与网关冲突；
  - WebUI 精细化交互体验与严密表单校验：
    - IPv6 外网设置：在中继模式下保留外网 DHCPv6 模式选择（Stateless RA / Stateful DHCPv6 / Both），自动收敛隐藏静态网关/地址等冗余项；
    - IPv6 内网设置：在中继模式下智能隐藏整个内网配置表格（tbl_ip6_lan），杜绝误配本地广播导致网络异常；
    - NAPT66 页面归集、Tooltip 气泡交互与严格表单校验：
      - 将“启用 napt66”从原先杂乱的服务页面规范迁移至 IPv6 专属配置页；
      - 遵照 WebUI 规范在 help.js 中扩展 helpcontent[25]，将互斥说明封装为原生 help_tooltip 悬浮提示框（鼠标悬停即刻优雅弹出交互式气泡）；
      - 在 validForm 中实施严格表单校验，若在 Relay 模式下开启 NAPT66 则在提交时前端拦截并弹出警示，杜绝互斥配置；
  - 经技术验证与对比，彻底剔除二层网桥透传（Passthrough / ebtables broute）及相关试验性内核模块，保持系统内核纯净高效。
- 修复 Transmission Web UI (v0.1.9) 种子列表在拖高详情栏后缩放窗口导致高度塌陷与 Canvas 残影重叠异常：
  - 根因分析：当用户将底部详情面板（detailHeight）向上拉至最大（600px）并缩小窗口时，列表高度计算公式 bodyHeight - headerHeight - footerHeight - detailHeight 击穿为负数，导致详情面板通过 translateY 向上倒冲覆盖列表，同时 Canvas 画布高度非正数引发 clearRect 擦除失效、虚拟滚动坐标塌陷及旧帧残影无限叠加。
  - 修复方案：
    - 列表高度兜底防护：在 DashboardView 的 listheight 计算逻辑中加入 Math.max(120, ...) 最小高度保障，杜绝负数高度击穿。
    - 详情栏拖拽上限动态联动：将详情面板的 max-container-height 动态绑定至当前窗口可用高度（Math.max(120, bodyHeight - headerHeight - footerHeight - 120)），彻底防止拖拽挤压。
    - 虚拟列表与 Canvas 画布防负数：在 CanvasList 与 CanvasMobileList 的 setClientHeight 以及 CanvasTableBody 的 canvasHeight 增加 Math.max(0, ...) 防御，确保清屏与绘制几何坐标严格处于合法区间。

2026/10/17：
- 升级重构电源指示灯（BOARD_GPIO_LED_POWER）生命律动与负载感应系统（BW050-A 等机型支持）：
  - 在 detect_link 中实现多维度的智能心跳指示逻辑，完全复用原生 2 秒主事件循环，零系统唤醒开销：
    - 模式 2 (默认)：基于 CPU 瞬时使用率（/proc/stat）动态心跳（空闲 <20% 温和微闪眨眼、中载 20%~75% 经典 Lub-Dub 双心跳、重载 >=75% 警戒急促连闪）
    - 模式 3：基于系统 1 分钟平均负载（sysinfo loadavg）动态心跳（空闲 <0.3 微闪、中载 0.3~1.0 双心跳、重载 >=1.0 急促连闪）
    - 模式 4：极简均匀慢频呼吸灯（2s 亮 2s 灭）
    - 模式 1：传统常亮（按钮事件）
    - 模式 0：彻底禁用（关闭）
  - 全面联动 WebUI“系统微调”页面（Advanced_Tweaks_Content.asp）与多语言字典（CN/EN/RU/UK），提供全套模式下拉选单，默认值（defaults.c）切换为模式 2
  - 完美兼容夜间定时休眠机制（led_front_t），休眠期间与关闭模式下完全静默熄灭
  - 修复 BW050-A 内核配置缺失的 SFE 依赖占位项（# CONFIG_SHORTCUT_FE is not set、# CONFIG_NF_CONNTRACK_CHAIN_EVENTS is not set），消除 non-interactive Kconfig 提示中断
- 升级 Transmission Web UI 至最新版 v0.1.9（jianxcao/transmission-web）：
  - 彻底去除 PWA 相关后台服务与缓存文件（移除 sw.js、workbox、manifest），避免嵌入式环境缓存异常
  - 移除所有冗余的大体积位图图标（pwa-512.png、pwa-192.png、apple-touch-icon.png），仅保留 2.7KB 矢量 transmission.svg 作为通用图标
  - 为 ROMFS 净省约 330KB 存储空间，保持现代 WebUI 极致轻量与高响应度
- 优化与重构小米路由器 Pro（MI-R3P / MI-R3P-spi）LED 指示灯系统：
  - 修正原配置将网口速率灯误定义为 LAN/WAN/USB 指示灯（GPIO 13/14/16）的错误，移除无效的宏定义
  - 修正 BOARD_NUM_ETH_LEDS 为 1，对应网口单绿灯硬件 PHY 模式，消除 WebUI 幽灵黄灯配置项
  - 适配 4 端口自适应速率指示灯架构（WAN: GPIO 14, LAN1: GPIO 13, LAN2: GPIO 16, LAN3: GPIO 15）
  - 前面板 RGB 三色状态指示灯防混色支持：在 rc 中对 BOARD_MIR3P 启用红 (6)、黄 (10) 通道熄灭保护，确保运行阶段纯蓝发光
  - 补全 MI-R3P-spi 内核配置中缺失的 RALINK_CPUSLEEP、RT2880_FLASH_AUTO 依赖项与 EXT4_USE_FOR_EXT23 兼容项
- 优化与重构小米路由器 3G（MI-R3G）LED 指示灯系统：
  - 修正原配置将 LAN1 误定义为 USB 指示灯（GPIO 14）的错误，移除无效的 USB LED 宏
  - 修正 BOARD_NUM_ETH_LEDS 为 1，对应网口单绿灯硬件 PHY 模式，消除 WebUI 幽灵黄灯配置项
  - 增加通用网口速率指示灯框架（BOARD_GPIO_LED_*_SPEED）：
    - 采用方案1速率指示逻辑：1000M 全双工常亮、100M 熄灭、10M 慢速闪烁，半双工异常时快闪报警，未连线熄灭
    - 优化轮询开销：挂载至链路变动中断与阶段性时钟，仅在 link 变化或闪烁周期按需刷新，无链路变动时零性能开销
  - 解决前面板三色状态 LED（红 6 / 蓝 8 / 黄 10）混色问题：系统运行中关闭红/黄通道，纯净点亮主蓝灯
  - 调整与统一网口速率指示灯为 2 秒原生节拍体系（降速防眩光，原生 2s 轮询合并）：
    - 千兆全双工（1000M FD）：熄灭
    - 百兆全双工（100M FD）：常亮
    - 十兆全双工（10M FD）：慢闪（2s 亮 2s 灭，周期 4s）
    - 百兆半双工（100M HD）：慢闪（2s 亮 4s 灭，周期 6s）
    - 十兆半双工（10M HD）：慢闪（2s 亮 6s 灭，周期 8s）
    - 未插线（No Link）：熄灭
  - 简化 detect_link 架构，直接复用原版 2s（DL_POLL_INTERVAL）主事件循环计数器驱动 LED 闪烁周期，零额外唤醒开销
  - 扩充通用网口速率指示灯抽象（BOARD_GPIO_LED_*_SPEED），补齐 LAN3 / LAN4 宏定义与事件处理
  - 网口速率指示灯联动 WebUI “网卡端口 LED (绿)”配置：当 ether_led0 关闭（11）或处于定时休眠（!led_ether_t）时，速率指示灯（琥珀色）同步强制熄灭并不再亮起；并在 update_ether_leds 中增加即时刷新通知
- 优化 USB 指示灯（front_led_usb=3）联动机制：基于 USB 存储挂载状态控制，未插入设备或安全弹出存储设备后彻底熄灭，挂载就绪后常亮，读写数据传输时闪烁
- 在存储设备安全弹出（ejusb / safe_remove_stor_device）及热插拔事件中增加指示灯即时刷新通知（notify_leds_detect_link）
- 修复 WebUI 中文翻译（CN.dict）中 USB 指示灯模式描述颠倒的错误（模式2更正为“USB 设备已连接”，模式3更正为“USB 存储读写活动 (挂载常亮，读写闪烁)”）
- 将默认 front_led_usb 初始值由 1 调整为 3
----------------------------------------------------------

2026/07/18：
- 修复 MI-R3G 5G WiFi 异常（移除无效的 MT76X3_AP 驱动，避免抢走 5G 设备）
- 修正 MI-R3P-spi 内核配置：NAND→SPI 闪存、ETH_LEDS、移除无效 STORE_PART_SIZ
- 修复 MI-R3P-spi SPI 闪存 MTD_STORE_PART_SIZ 缺失导致编译报错
- 确认 BW050-A 5G FEM SKY85703-11 需使用 eeprom(1).bin 校准数据
- 修复 minidlna 缺 ffmpeg 共享库问题（libavformat/libavcodec/libavutil）
- BW050-A 启用 microSD 卡槽支持（MTK MMC 驱动）
- 创建 BW050-A_SUMMARY.md 设备配置总览文档
----------------------------------------------------------
- 升级 aria2 到 1.37.0
- 升级 AriaNg (aria2 网页控制台) 到 1.3.14
- 升级 Transmission Web Control 到 1.6.1-update1（最终版）
- 修复 transmission 静态链接 libstdc++（解决启动报错）
- 升级 OpenSSL 到 1.1.1w（1.1.1 系列最终版，修复多个 CVE 漏洞）
- 新增 Transmission 新版 Web UI 编译选项（CONFIG_FIRMWARE_INCLUDE_TRANSMISSION_WEB_NEW），来自 https://github.com/jianxcao/transmission-web
- 修复 transmission 缺 libstdc++.so.6 问题（将工具链库文件打包进固件）

2026/07/17：
----------------------------------------------------------
- 基于 RT-AC51U 配置文件适配 BW050-A 路由器（MT7620+MT7610），设备标签为 “专用设备BW050-A”

很久以前：
----------------------------------------------------------
- 适配 MAC2600R 路由器
- 适配 TCL-T1 路由器（MT7628）
