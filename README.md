# TZgram

> **本项目并非 Telegram 官方版本**，是基于 [Telegram 官方桌面端][tdesktop] 源码的二次开发，与 Telegram FZ-LLC 无关。

由 **ONEBOAT 同舟** 二次开发并开源，应用名为 **TZgram**。

* 源码仓库：<https://github.com/TZerjiuma/TZgram>
* 有问题请联系：[@TZsandy_bot](https://t.me/TZsandy_bot)

## 相对官方版本的改动

1. **去除登录账户数量限制**：账户上限 3 个（官方 Premium 为 6 个）。
2. **新增账户栏**：主界面侧边增加 `AccountsRail`，方便多账户之间切换。
3. **关闭文件自动下载**：文件、音乐等自动下载上限置为 0，可在「设置 → 数据与存储 → 自动下载」中开启。
4. **默认媒体下载上限 1 MB**：官方默认 8 MB。
5. **关闭媒体自动播放**：GIF、视频、视频消息与贴纸默认不自动播放。
6. **关闭循环播放动画**：动画贴纸与动画表情默认只播放一次。
7. **默认开启省电模式**：贴纸、表情、聊天特效、通话、界面动画等省电选项默认全部开启。
8. **默认简体中文语言包**：默认使用 `zh-hans`，可在「设置 → 语言」中切换。
9. **关闭自动更新**：编译期关闭自动更新，不再有更新检查和新版本提示。

## 未做修改的部分

**用户消息功能与隐私相关部分完全沿用官方实现，未做任何修改。** 包括消息收发与编辑、隐私与安全设置、群组与频道权限、两步验证、密钥与本地数据存储等。未配置商业证书，安装会有风险提示，拒绝任何碰瓷行为。

## 编译

依赖 Visual Studio 2022、CMake、Qt 6.11 及官方文档 [Building the desktop version][tdesktop-build] 中列出的其他依赖，编译步骤与官方一致。

## 许可

源码基于 [Telegram 官方桌面端][tdesktop] 发布，遵循 **GPLv3 with OpenSSL exception**，许可证见 [LICENSE][license]；二次开发部分的改动同样以该许可证发布。

[//]: # (LINKS)
[tdesktop]: https://github.com/telegramdesktop/tdesktop
[tdesktop-build]: https://github.com/telegramdesktop/tdesktop#building-the-desktop-version
[license]: LICENSE
