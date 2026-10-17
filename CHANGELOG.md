更新日志

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
