/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include <memory>

#include <QString>

class QNetworkAccessManager;

namespace Core {

// Fetches a fresh list of free MTProto proxies and replaces the whole proxy
// pool with it. Triggered manually from the proxy settings. See
// `core/free_proxy_manager.cpp`.
class FreeProxyManager final {
public:
	FreeProxyManager();
	~FreeProxyManager();

	void refresh();

private:
	void applyList(const QString &text);

	std::unique_ptr<QNetworkAccessManager> _manager;

};

} // namespace Core
