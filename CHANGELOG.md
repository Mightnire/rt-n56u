更新日志

2026/10/17：
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
