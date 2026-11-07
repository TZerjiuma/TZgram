/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "base/const_string.h"

#define TDESKTOP_REQUESTED_ALPHA_VERSION (0ULL)

#ifdef TDESKTOP_ALLOW_CLOSED_ALPHA
#define TDESKTOP_ALPHA_VERSION TDESKTOP_REQUESTED_ALPHA_VERSION
#else // TDESKTOP_ALLOW_CLOSED_ALPHA
#define TDESKTOP_ALPHA_VERSION (0ULL)
#endif // TDESKTOP_ALLOW_CLOSED_ALPHA

// used in Updater.cpp and Setup.iss for Windows
constexpr auto AppId = "{53F49750-6209-4FBF-9CA8-7A333C87D1ED}"_cs;
constexpr auto AppNameOld = "Telegram Win (Unofficial)"_cs;
constexpr auto AppName = "TZgram"_cs;
constexpr auto AppFile = "TZgram"_cs;
constexpr auto AppVersion = 7005002;
constexpr auto AppVersionStr = "7.5.2";
constexpr auto AppBetaVersion = false;
constexpr auto AppAlphaVersion = TDESKTOP_ALPHA_VERSION;

// The branding shown in the interface.
constexpr auto AppDesktopSuffix = "ONEBOAT"_cs;
constexpr auto AppDeveloperText = "本应用基于 Telegram Desktop 官方开源代码二次开发，仅增加更多账号同时登录，\n未修改与服务器的通信协议，未植入任何消息采集、上传或篡改逻辑，\n修改部分源码已发布在 Github 仓库公开，免费分享，拒绝碰瓷。"_cs;
constexpr auto AppChannelUrl = "https://oneboat.one"_cs;
constexpr auto AppRepoUrl = "https://github.com/TZerjiuma/TZgram"_cs;
