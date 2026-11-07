/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "tray_accounts_menu.h"

#include "base/weak_ptr.h"
#include "base/qt/qt_key_modifiers.h"
#include "core/application.h"
#include "data/data_peer.h"
#include "data/data_user.h"
#include "info/profile/info_profile_values.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "styles/style_window.h"
#include "ui/dynamic_thumbnails.h"

namespace Core::TrayAccountsMenu {

void SetupChangesSubscription(Fn<void()> callback, rpl::lifetime &lifetime) {
	const auto accountSessionsLifetime = lifetime.make_state<rpl::lifetime>();
	const auto watchAccountSessions = [=] {
		accountSessionsLifetime->destroy();
		for (const auto &[index, account] : Core::App().domain().accounts()) {
			account->sessionChanges(
			) | rpl::on_next([=](Main::Session*) {
				callback();
			}, *accountSessionsLifetime);
		}
	};
	Core::App().domain().accountsChanges() | rpl::on_next([=] {
		watchAccountSessions();
		callback();
	}, lifetime);
	watchAccountSessions();
}

} // namespace Core::TrayAccountsMenu
