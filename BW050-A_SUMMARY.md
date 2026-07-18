# BW050-A 路由器配置总结

## 硬件信息
- **SoC**: MT7620A (MIPS 24KEc)
- **5G 芯片**: MT7610E / MT7612E
- **架构**: 2.4G 2T2R + 5G 1T1R
- **存储**: SPI Flash + microSD 卡槽
- **网口**: 百兆 (100Mbps)
- **USB**: 1 个 USB 2.0

## GPIO 配置 (board.h)
| 功能 | GPIO | 说明 |
|------|------|------|
| BTN_RESET | 13 | Reset 按键，长按 5s 恢复出厂 |
| BTN_WPS | 12 | WPS/多功能按键 |
| LED_WIFI | 72 | WiFi 指示灯 (WLED 硬件引脚) |
| LED_POWER | 25 | 电源指示灯 |
| LED_USB | 39 | USB 指示灯 |
| ETH_LEDS | 1 | 单通道网口 LED |

## 固件配置 (templates/BW050-A.config)

### WiFi 驱动
- 2.4G: v3.0 (MT7620)
- 5G: v3.0 (MT7610E/MT7612E)
- SKU 文件: SingleSKU_CN.dat (2.4G), SingleSKU_5G_CE.dat (5G fallback)

### 文件系统支持
- FAT/FAT32, exFAT, NTFS (ufsd), EXT4, FUSE
- microSD: MTK MMC 驱动已启用

### 网络功能
- IPv6: 已启用
- SFE: 未启用
- napt66: 未启用

### 已编译组件
- OpenSSL 1.1.1w
- aria2 1.37.0 + AriaNg
- Transmission 3.00 + Web Control + 新版 WebUI 选项
- OpenSSH, OpenVPN, SFTP, Samba, FTP

### USB 网络共享
内核模块: rndis_host, cdc_ether, cdc_ncm, qmi_wwan

## 编译选项备忘

```
# 启用 SFE 加速转发
CONFIG_FIRMWARE_INCLUDE_SFE=y

# 启用 napt66 IPv6 NAT
CONFIG_FIRMWARE_INCLUDE_NAPT66=y

# Transmission WebUI (三选一)
#CONFIG_FIRMWARE_INCLUDE_TRANSMISSION_WEB_NEW=y   # 新版 PWA
CONFIG_FIRMWARE_INCLUDE_TRANSMISSION_WEB_CONTROL=y # 旧版 Web Control
# (都不设 = transmission 自带)
```

## 关键修复记录
1. transmission 缺 libstdc++.so.6 → 打包工具链库
2. aria2 1.35.0 → 1.37.0: patch 直接适配
3. OpenSSL 1.1.1k → 1.1.1w: patch 有 minor offset
4. microSD 卡槽: 内核启用 MTK MMC 驱动

## 文件结构
```
configs/boards/BW050-A/
├── board.h           # GPIO/硬件定义
├── board.mk          # 编译标志
├── kernel-3.4.x.config  # 内核配置
└── SingleSKU*.dat    # WiFi 区域功率参数

configs/templates/
└── BW050-A.config    # 固件功能配置
```

## Git 仓库
- 远程: https://github.com/Mightnire/rt-n56u.git
- 提交前需: `git pull --rebase origin master`
- 推送需 GitHub Token
