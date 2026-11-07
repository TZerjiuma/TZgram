/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "core/free_proxy_manager.h"

#include "boxes/connection_box.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "mtproto/mtproto_proxy_data.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkProxy>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>

namespace Core {
namespace {

constexpr auto kFreeProxyListUrl =
	"https://raw.githubusercontent.com/0xRadikal/Free-Telegram-Proxy/main/faketls/tg.txt";

} // namespace

FreeProxyManager::FreeProxyManager()
: _manager(std::make_unique<QNetworkAccessManager>()) {
	// The list must be fetched directly, not through the current proxy.
	_manager->setProxy(QNetworkProxy::NoProxy);
}

FreeProxyManager::~FreeProxyManager() = default;

void FreeProxyManager::refresh() {
	const auto request = QNetworkRequest(QUrl(QLatin1String(kFreeProxyListUrl)));
	const auto reply = _manager->get(request);
	QObject::connect(reply, &QNetworkReply::finished, [=] {
		reply->deleteLater();
		if (reply->error() != QNetworkReply::NoError) {
			return;
		}
		const auto text = QString::fromUtf8(reply->readAll());
		if (text.isEmpty()) {
			return;
		}
		applyList(text);
	});
}

void FreeProxyManager::applyList(const QString &text) {
	auto proxies = std::vector<MTP::ProxyData>();
	const auto lines = text.split('\n', Qt::SkipEmptyParts);
	proxies.reserve(lines.size());
	for (const auto &line : lines) {
		auto proxy = ProxiesBoxController::ProxyFromLink(line);
		if (proxy && (proxy.type == MTP::ProxyData::Type::Mtproto)) {
			proxies.push_back(std::move(proxy));
		}
	}
	if (proxies.empty()) {
		return;
	}
	auto &settings = Core::App().settings().proxy();
	settings.setList(std::move(proxies));
	Core::App().setCurrentProxy(
		settings.list().front(),
		MTP::ProxyData::Settings::Enabled);
}

} // namespace Core
