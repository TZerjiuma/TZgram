# TZ-telegram 桌面端

> **本项目并非 Telegram 官方版本**，是基于 [Telegram 官方桌面端][telegram_desktop] 源码的二次开发，与 Telegram FZ-LLC 无关。

由 **ONEBOAT 同舟** 二次开发并开源，登录界面与「设置 → 关于」均已标注二次开发信息。

* 开源仓库：<https://github.com/TZerjiuma/tz-tdesktop>
* Telegram 频道：[@oneboat](https://t.me/oneboat)

## 相对官方版本的改动

1. **去除登录账户数量限制**：账户上限 3 个（Premium 6 个）。
2. **微调 UI**：主界面侧边新增账户栏（`AccountsRail`），方便多账户之间切换。
3. **关闭默认的文件自动下载**：文件、音乐等的自动下载上限置为 0，可在「设置 → 数据与存储 → 自动下载」中开启。
4. **默认媒体下载上限 1 MB**：由官方的 8 MB 调整为 1 MB，GIF、视频的自动播放默认关闭。
5. **关闭耗电选项**：省电（电池）与动画相关的耗电选项默认全部关闭。
6. **品牌标注**：登录界面与「设置 → 关于」标注二次开发信息与开源地址。
7. **默认简体中文语言包**：默认使用 `zh-hans`，可在「设置 → 语言」中切换。
8. **关闭自动更新**：编译期关闭自动更新，不再启动更新检查，也没有新版本提示。

## 未做修改的部分

**用户消息功能与隐私相关部分完全沿用官方实现，未做任何修改。** 包括消息收发与编辑、隐私与安全设置、群组与频道权限、两步验证、密钥与本地数据存储等。

## 构建

沿用官方版本的构建体系，说明见 [docs/building-win.md](docs/building-win.md) 等文档。

## 许可

源码基于 [Telegram 官方桌面端][telegram_desktop] 发布，遵循 **GPLv3 with OpenSSL exception**，许可证见 [LICENSE][license]；二次开发部分的改动同样以该许可证发布。

[//]: # (LINKS)
[telegram_desktop]: https://github.com/telegramdesktop/tdesktop
[license]: LICENSE
