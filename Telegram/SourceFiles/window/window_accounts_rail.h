/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "ui/rp_widget.h"
#include "ui/widgets/scroll_area.h"

namespace Window {

class SessionController;

class AccountsRail final : public Ui::RpWidget {
public:
	AccountsRail(
		not_null<Ui::RpWidget*> parent,
		not_null<SessionController*> session);
	~AccountsRail();

	[[nodiscard]] static int Width();

private:
	class Inner;

	void paintEvent(QPaintEvent *e) override;

	const not_null<SessionController*> _session;
	Ui::ScrollArea _scroll;
	QPointer<Inner> _inner;

};

} // namespace Window
