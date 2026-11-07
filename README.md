# TZgram

> **本项目并非 Telegram 官方版本**，是基于 [Telegram 官方桌面端][tdesktop] 源码的二次开发，与 Telegram FZ-LLC 无关。

由 **ONEBOAT 同舟** 二次开发并开源，应用名为 **TZgram**。

* 源码仓库：<https://github.com/TZerjiuma/TZgram>
* 有问题请联系：[@TZsandy_bot](https://t.me/TZsandy_bot)

## 相对官方版本的改动

1. **去除登录账户数量限制**：账户上限 3 个（官方 Premium 为 6 个）。
2. **新增账户栏**：主界面侧边增加 `AccountsRail`，按头像展示账户，上下拖动排序，横向拖出可拆分成独立窗口。
3. **未读计数只保留在账户栏**：消息未读计数与红点只在侧面栏（账户栏）显示，托盘图标、主菜单等其他位置的未读计数均已移除。
4. **关闭文件自动下载**：文件、音乐等自动下载上限置为 0，可在「设置 → 数据与存储 → 自动下载」中开启。
5. **默认媒体下载上限 1 MB**：官方默认 8 MB。
6. **关闭媒体自动播放**：GIF、视频、视频消息与贴纸默认不自动播放。
7. **关闭循环播放动画**：动画贴纸与动画表情默认只播放一次。
8. **默认开启省电模式**：贴纸、表情、聊天特效、通话、界面动画等省电选项默认全部开启。
9. **默认简体中文语言包**：默认使用 `zh-hans`，可在「设置 → 语言」中切换。
10. **关闭自动更新**：编译期关闭自动更新，不再有更新检查和新版本提示。
11. **消息翻译按钮**：消息气泡旁新增翻译按钮，点击后在原消息下方内联显示译文（官方为右键菜单/顶部翻译条）。
12. **输入框翻译**：输入框按 `Ctrl+T`（或右键菜单「翻译」）把整段内容翻译成目标语言并替换。
13. **翻译默认设置**：默认跳过简体中文、消息翻成中文；新增「翻译成」设置（默认英语，用于输入框翻译）。

## 未做修改的部分

**用户消息功能与隐私相关部分完全沿用官方实现，未做任何修改。** 包括消息收发与编辑、隐私与安全设置、群组与频道权限、两步验证、密钥与本地数据存储等。未配置商业证书，安装会有风险提示，拒绝任何碰瓷行为。

## 编译

依赖 Visual Studio 2022、CMake、Qt 6.11 及官方文档 [Building the desktop version][tdesktop-build] 中列出的其他依赖。

1. 创建 `.env` 文件，写入：
   ```
   api_id=你的api_id
   api_hash=你的api_hash
   version=7.5.1
   ```
2. 运行 `build.bat` 编译（增量）
3. 需要签名打包时运行 `build.bat package`（可选）
4. 改了 `version` 的话，同步一次版本文件（脚本在仓库外的 `tzgram-signing/` 里）：
   ```
   python tzgram-signing\tzgram-env.py --sync
   ```
   它把 `.env` 里的版本号写进仓库的 5 个版本文件（程序文件名、程序里显示的版本、
   PE 资源里的 FileVersion）。不跑这一步的话，程序还是旧版本号。

## 签名与打包

`build.bat package` 会依次：编译 → 签名主程序 → 打安装包 → 签名安装包。

签名密钥**不在仓库里**（是真凭据，不能进 git），换机器需手动搬运。需要的东西：

1. 仓库外的签名目录 `tzgram-signing/`：
   - `codesign.pfx` —— 签名私钥
   - `TZgram.cer` —— 公钥证书
   - `sign-and-package.bat` —— 签名 + 打包脚本
2. `.env` 里的签名配置（键名，值为本机路径）：
   ```
   sign_pfx=D:\bot\telegram\tzgram-signing\codesign.pfx
   sign_password=你的签名密码
   sign_cert=D:\bot\telegram\tzgram-signing\TZgram.cer
   sign_timestamp=http://timestamp.digicert.com
   signtool=C:\Program Files (x86)\Windows Kits\10\bin\...\x64\signtool.exe
   iscc=C:\Program Files\Inno Setup 7\ISCC.exe
   ```
3. 外部工具：Inno Setup 7（打安装包）、signtool（Windows SDK 自带）。

## 许可

源码基于 [Telegram 官方桌面端][tdesktop] 发布，遵循 **GPLv3 with OpenSSL exception**，许可证见 [LICENSE][license]；二次开发部分的改动同样以该许可证发布。

[//]: # (LINKS)
[tdesktop]: https://github.com/telegramdesktop/tdesktop
[tdesktop-build]: https://github.com/telegramdesktop/tdesktop#building-the-desktop-version
[license]: LICENSE
