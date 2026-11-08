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
constexpr auto AppVersion = 7005003;
constexpr auto AppVersionStr = "7.5.3";
constexpr auto AppBetaVersion = false;
constexpr auto AppAlphaVersion = TDESKTOP_ALPHA_VERSION;

// The branding shown in the interface.
constexpr auto AppDesktopSuffix = "ONEBOAT"_cs;
constexpr auto AppDeveloperText = "声明：TZgram 是基于 Telegram 官方源码二次开发，源码可在 Github 获取。"_cs;
constexpr auto AppChannelUrl = "https://oneboat.one"_cs;
constexpr auto AppRepoUrl = "https://github.com/TZerjiuma/TZgram"_cs;
