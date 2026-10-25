# rt-n56u (Padavan) 路由器固件定制与增强版

本项目基于优秀的 Padavan 开源固件（继承自 hanwckf/rt-n56u 与 padavan-ng 分支），专为现代路由器提供更加稳定、轻量且强大的系统支持，重点增强了安全组件、下载套件、指示灯系统与新型号适配。

> [!NOTE]
> 本项目源码供个人学习与技术探索使用。刷机有风险，请务必提前备份好原厂 EEPROM 与 Factory 分区！

---

### 项目关键特性与深度优化

- **核心网络与安全组件**
  - **OpenSSL**：升级至 1.1.1 最终 LTS 版本 `1.1.1w`，修复多项历史 CVE 安全漏洞。
  - **IPv6 穿透强化**：优化 `6relayd` 动态网络接口解析与 `ip6tables` FORWARD 穿透链，IPv6 状态稳定可靠。
- **现代化离线下载套件**
  - **Transmission**：升级至稳定性最佳的 C 语言核心 `3.07.1`，支持 64 位大文件（LFS，解决 2GB+ 文件下载崩溃），优化内核 UDP 缓冲；集成极简现代 WebUI（去 PWA 冗余设计，立省 ~330KB 闪存）。
  - **Aria2**：集成最新 `AriaNg` 前端，初始模板默认开启 `allow-piece-length-change=true`，根治断点续传与动态重定向时的 `ErrorCode=27`（分片长度不匹配）报错。
- **交互与硬件指示灯系统**
  - **智能 POWER 负载心跳律动 (`CONFIG_FIRMWARE_INCLUDE_POWER_LED_HEARTBEAT`)**：
    - 支持瞬时 CPU 负载与系统平均负载（LoadAvg）双模式感应。
    - 2 秒拟真节拍：低负载温和微眨（亮1.92s/灭80ms）、中载 Lub-Dub 双跳心跳、高负载急促脉冲。
    - 零系统唤醒开销，支持 WebUI 独立开关与夜间自动熄灭，小内存机型可按需编译。
  - **网口速率指示**：小米路由器 3G/Pro 等机型适配千兆网口自适应指示灯（千兆熄灭、百兆常亮、十兆非对称慢闪）。
  - **USB 状态联动**：设备挂载常亮、读写闪烁、WebUI 安全弹出后自动熄灭。
- **WebUI 与多语言本地化**
  - 完整补齐并精炼 14 种语言（含简中、繁中、英、俄、乌、法、德等）的 Tweaks 与前置 LED 控制词典，消除 `<#Tweaks...#>` 原始未翻译标签。

---

### 支持机型列表（除原厂支持外的适配列表）

> 标注 `(USB)` 的机型原生支持外接 USB 存储与打印服务。

| 芯片平台 / 品牌 | 支持型号 |
| :--- | :--- |
| **MT7620 平台** | **BW050-A** (MT7620A+MT7610E, 外置FEM, microSD, USB)<br>**NETGEAR PR2000** (Trek N300, 双Config分区/硬件开关联动, USB)<br>PSG1208、PSG1218、PSG1218_nano、OYE-001 (USB)、NEWIFI-MINI (USB)、MI-MINI (USB)、MI-3 (USB)、MI-NANO、HC5661A、HC5761A (USB)、HC5861B、360P2 (USB)、5K-W20 (USB)、RT-AC51U |
| **MT7621 平台** | **MI-R3G** (RGB防混色/网口慢闪, USB)<br>**MI-R3P-spi** (16MB SPI Flash 改版, 4网口指示灯, USB)<br>**MI-R3P / MI-R3P-breed** (NAND, USB)<br>K2P、K2P_nano、K2P-USB (USB)、NEWIFI3 (USB)、B70 (USB)、A3004NS (USB)、DIR-878、DIR-882 (USB)、MR2600 (USB)、RM2100、R2100、CR660x (CR6606/CR6608/CR6609)、JCG-836PRO (USB)、JCG-AC860M (USB)、JCG-Q20、JCG-Y2 (USB)、E8820V2 (USB)、ZTE_E8820S (USB)、MSG1500 (USB)、WR1200JS (USB)、XY-C1 (USB)、MAC2600R |
| **MT7628 平台** | TCL-T1、MI-3C、MI-4、MI-R4A、WDR7300 |
| **MT7615 平台** | NETGEAR-CHJ (R6260, R6350, R6850, WAC124)<br>NETGEAR-BZV (R6800, R6700-v2, R7200, Nighthawk AC2400)<br>R6220 (USB)、RT-AC1200GU (USB) |

---

### 快速开始与固件编译

#### 1. 准备编译环境 (以 Ubuntu / Debian 为例)

```bash
sudo apt update
sudo apt install unzip libtool-bin curl cmake gperf gawk flex bison nano xxd \
    fakeroot kmod cpio git python3-docutils gettext automake autopoint \
    texinfo build-essential help2man pkg-config zlib1g-dev libgmp3-dev \
    libmpc-dev libmpfr-dev libncurses5-dev libltdl-dev wget libc-dev-bin
```

#### 2. 获取源码

```bash
git clone https://github.com/Mightnire/rt-n56u.git /opt/rt-n56u
```

#### 3. 准备交叉工具链

开发目录已包含基于 crosstool-NG 的构建脚本：

```bash
cd /opt/rt-n56u/toolchain-mipsel
./clean_toolchain
./build_toolchain
```

#### 4. 开始构建固件

固件配置文件位于 `trunk/configs/templates/<TARGET_BOARD>.config`。以 `BW050-A` 为例：

```bash
cd /opt/rt-n56u/trunk

# 切换机型或清理旧构建
sudo ./clear_tree

# 一键构建固件镜像
sudo fakeroot ./build_firmware_modify BW050-A
```

构建成功后，固件文件（`.trx`）将输出在 `trunk/images/` 目录下。

---

### 鸣谢与致敬

- [Padavan](https://bitbucket.org/padavan/rt-n56u)
- [hanwckf/rt-n56u](https://github.com/hanwckf/rt-n56u)
- [padavan-ng](https://gitlab.com/padavan-ng/padavan-ng)
- [gorden5566/padavan](https://github.com/gorden5566/padavan)
- [stefantalpalaru/transmission-og](https://github.com/stefantalpalaru/transmission-og)
