更新日志

2026/07/18：
- 修复 MI-R3G 5G WiFi 异常（移除无效的 MT76X3_AP 驱动，避免抢走 5G 设备）
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
