/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "window/window_accounts_rail.h"

#include "window/window_session_controller.h"
#include "window/window_controller.h"
#include "mainwindow.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "core/application.h"
#include "core/core_settings.h"
#include "data/data_changes.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "data/data_user.h"
#include "mtproto/mtp_instance.h"
#include "base/algorithm.h"
#include "base/invoke_queued.h"
#include "base/weak_qptr.h"
#include "lang/lang_tag.h"
#include "settings/sections/settings_information.h"
#include "ui/painter.h"
#include "ui/ui_utility.h"
#include "ui/unread_badge_paint.h"
#include "styles/style_window.h"

#include <QtCore/QMimeData>
#include <QtGui/QCursor>
#include <QtWidgets/QApplication>

#include <cstdlib>

namespace Window {
namespace {

[[nodiscard]] int RowFull() {
	return st::windowAccountsRailPhoto + 2 * st::windowAccountsRailSkip;
}

// The vertical distance that turns a click into a drag of the account.
constexpr auto kDragThreshold = 8;

// The horizontal distance that turns a click into a drag of the account
// out of the window.
constexpr auto kDetachThreshold = 16;

} // namespace

class AccountsRail::Inner final : public Ui::RpWidget {
public:
	Inner(QWidget *parent, not_null<AccountsRail*> rail);

	void rebuild();

private:
	struct Entry {
		not_null<Main::Account*> account;
		Ui::PeerUserpicView view;
	};

	void paintEvent(QPaintEvent *e) override;
	void mouseMoveEvent(QMouseEvent *e) override;
	void mousePressEvent(QMouseEvent *e) override;
	void mouseReleaseEvent(QMouseEvent *e) override;
	void leaveEventHook(QEvent *e) override;

	[[nodiscard]] QRect rowRect(int index) const;
	[[nodiscard]] int indexAt(QPoint position) const;
	[[nodiscard]] int rowsHeight() const;
	[[nodiscard]] bool addRow(int index) const;
	void updateHovered(QPoint position);
	void paintEntry(Painter &p, int index, QRect rect);
	void paintBadge(Painter &p, const Entry &entry, QRect rect);
	void paintAdd(Painter &p, QRect rect);
	void activate(int index, Qt::KeyboardModifiers modifiers);
	void addAccount();

	void startDrag();
	void updateDrag(QPoint position);
	void finishDrag(QPoint position);
	void saveOrder();

	void startDetach();
	void performDetach();
	void finishDetach(not_null<Main::Account*> account, QPoint position);
	void detachAccount(not_null<Main::Account*> account, QPoint position);
	void mergeAccount(
		not_null<Main::Account*> account,
		not_null<Controller*> target);
	[[nodiscard]] Main::Account *otherAccount(
		not_null<Main::Account*> account) const;
	static void moveWindow(not_null<::MainWindow*> widget, QPoint position);
	[[nodiscard]] static Controller *windowAt(QPoint position);

	const not_null<AccountsRail*> _rail;
	const not_null<SessionController*> _session;
	std::vector<Entry> _entries;
	rpl::lifetime _accountsLifetime;
	int _hovered = -1;
	int _pressed = -1;
	int _dragged = -1;
	int _detached = -1;
	QPoint _pressPosition;

};

AccountsRail::Inner::Inner(
	QWidget *parent,
	not_null<AccountsRail*> rail)
: Ui::RpWidget(parent)
, _rail(rail)
, _session(rail->_session) {
	setMouseTracking(true);
}

void AccountsRail::Inner::rebuild() {
	auto entries = std::vector<Entry>();
	const auto mine = &_session->session().account();
	for (const auto &account : Core::App().domain().orderedAccounts()) {
		if (!account->sessionExists()) {
			continue;
		}
		// Keep an account out of the rail if it lives in its own window,
		// but always keep the account that this window currently shows.
		if (account.get() != mine) {
			if (const auto showing = Core::App().separateWindowFor(
					Window::SeparateId(account))) {
				if (!showing->isMain()) {
					continue;
				}
			}
		}
		entries.push_back({ account });
	}
	_entries = std::move(entries);
	_hovered = _pressed = _dragged = _detached = -1;
	unsetCursor();
	setToolTip(QString());

	_accountsLifetime.destroy();
	for (const auto &entry : _entries) {
		const auto session = &entry.account->session();
		session->downloaderTaskFinished(
		) | rpl::on_next([=] {
			update();
		}, _accountsLifetime);
		session->changes().peerUpdates(
			session->user(),
			Data::PeerUpdate::Flag::Photo
		) | rpl::on_next([=](const Data::PeerUpdate &) {
			update();
		}, _accountsLifetime);
		session->data().unreadBadgeChanges(
		) | rpl::on_next([=] {
			update();
		}, _accountsLifetime);
	}

	resize(_rail->width(), rowsHeight());
	update();
}

bool AccountsRail::Inner::addRow(int index) const {
	return (index == int(_entries.size()));
}

int AccountsRail::Inner::rowsHeight() const {
	const auto full = RowFull();
	return st::windowAccountsRailTop
		+ int(_entries.size() + 1) * (full + st::windowAccountsRailGap)
		+ st::windowAccountsRailBottom;
}

QRect AccountsRail::Inner::rowRect(int index) const {
	const auto full = RowFull();
	const auto top = st::windowAccountsRailTop
		+ index * (full + st::windowAccountsRailGap);
	return QRect((width() - full) / 2, top, full, full);
}

int AccountsRail::Inner::indexAt(QPoint position) const {
	for (auto i = 0; i != int(_entries.size()) + 1; ++i) {
		if (rowRect(i).contains(position)) {
			return i;
		}
	}
	return -1;
}

void AccountsRail::Inner::updateHovered(QPoint position) {
	const auto index = indexAt(position);
	if (_hovered == index) {
		return;
	}
	_hovered = index;
	if (index >= 0 && !addRow(index)) {
		setToolTip(_entries[index].account->session().user()->name());
	} else {
		setToolTip(QString());
	}
	update();
}

void AccountsRail::Inner::paintEvent(QPaintEvent *e) {
	auto p = Painter(this);
	for (auto i = 0; i != int(_entries.size()); ++i) {
		paintEntry(p, i, rowRect(i));
	}
	paintAdd(p, rowRect(int(_entries.size())));
}

void AccountsRail::Inner::paintEntry(Painter &p, int index, QRect rect) {
	auto &entry = _entries[index];
	const auto photo = st::windowAccountsRailPhoto;
	const auto skip = st::windowAccountsRailSkip;
	const auto full = RowFull();
	const auto active = (entry.account.get()
		== &_session->session().account());

	if (index == _hovered || index == _dragged || index == _detached) {
		auto hq = PainterHighQualityEnabler(p);
		p.setPen(Qt::NoPen);
		p.setBrush(st::windowAccountsRailHover);
		p.drawEllipse(rect);
	}
	entry.account->session().user()->paintUserpicLeft(
		p,
		entry.view,
		rect.x() + skip,
		rect.y() + skip,
		full,
		photo);
	if (active) {
		const auto line = st::windowAccountsRailLine;
		const auto inset = line / 2.;
		const auto diameter = full - 2 * inset;
		auto hq = PainterHighQualityEnabler(p);
		auto pen = st::windowAccountsRailActive->p;
		pen.setWidthF(line);
		p.setPen(pen);
		p.setBrush(Qt::NoBrush);
		p.drawEllipse(QRectF(
			rect.x() + inset,
			rect.y() + inset,
			diameter,
			diameter));
	}

	paintBadge(p, entry, rect);
}

void AccountsRail::Inner::paintBadge(
		Painter &p,
		const Entry &entry,
		QRect rect) {
	const auto &session = entry.account->session();
	const auto count = session.data().unreadMessagesBadge();
	if (!count) {
		return;
	}
	auto badgeStyle = Settings::Badge::Style();
	const auto text = Lang::FormatCountToShort(count).string;
	const auto size = Ui::CountUnreadBadgeSize(text, badgeStyle);

	const auto photo = st::windowAccountsRailPhoto;
	const auto skip = st::windowAccountsRailSkip;
	const auto right = rect.x() + skip + photo;
	const auto bottom = rect.y() + skip + photo;

	Ui::PaintUnreadBadge(
		p,
		text,
		right,
		bottom - size.height(),
		badgeStyle);
}

void AccountsRail::Inner::paintAdd(Painter &p, QRect rect) {
	const auto photo = st::windowAccountsRailPhoto;
	const auto skip = st::windowAccountsRailSkip;
	const auto circle = QRect(
		rect.x() + skip,
		rect.y() + skip,
		photo,
		photo);

	if (_hovered == int(_entries.size())) {
		auto hq = PainterHighQualityEnabler(p);
		p.setPen(Qt::NoPen);
		p.setBrush(st::windowAccountsRailHover);
		p.drawEllipse(rect);
	}

	{
		auto hq = PainterHighQualityEnabler(p);
		p.setPen(Qt::NoPen);
		p.setBrush(st::windowAccountsRailAddBg);
		p.drawEllipse(circle);
	}

	const auto line = st::windowAccountsRailLine;
	const auto half = st::windowAccountsRailAddSize / 2.;
	const auto center = QPointF(circle.center()) + QPointF(0.5, 0.5);
	auto hq = PainterHighQualityEnabler(p);
	auto pen = st::windowAccountsRailAddFg->p;
	pen.setWidthF(line);
	pen.setCapStyle(Qt::RoundCap);
	p.setPen(pen);
	p.drawLine(
		QPointF(center.x() - half, center.y()),
		QPointF(center.x() + half, center.y()));
	p.drawLine(
		QPointF(center.x(), center.y() - half),
		QPointF(center.x(), center.y() + half));
}

void AccountsRail::Inner::mouseMoveEvent(QMouseEvent *e) {
	if (_pressed >= 0 && _dragged < 0 && _detached < 0) {
		const auto delta = e->pos() - _pressPosition;
		const auto dx = std::abs(delta.x());
		const auto dy = std::abs(delta.y());
		if (dx > dy) {
			// A horizontal drag pulls the account out of the window.
			if (dx >= kDetachThreshold) {
				startDetach();
			}
		} else if (dy >= kDragThreshold) {
			// A vertical drag reorders the accounts.
			startDrag();
		}
	}
	if (_dragged >= 0) {
		updateDrag(e->pos());
	} else {
		updateHovered(e->pos());
	}
}

void AccountsRail::Inner::mousePressEvent(QMouseEvent *e) {
	if (e->button() == Qt::LeftButton) {
		_pressed = indexAt(e->pos());
		_pressPosition = e->pos();
	}
}

void AccountsRail::Inner::mouseReleaseEvent(QMouseEvent *e) {
	if (e->button() != Qt::LeftButton) {
		return;
	}
	const auto pressed = _pressed;
	_pressed = -1;
	if (_detached >= 0) {
		// The account is being dragged out of the window.
		return;
	}
	if (_dragged >= 0) {
		finishDrag(e->pos());
		return;
	}
	if (pressed >= 0
		&& pressed == indexAt(e->pos())) {
		activate(pressed, e->modifiers());
	}
}

void AccountsRail::Inner::startDrag() {
	if (_pressed < 0 || addRow(_pressed)) {
		return;
	}
	_dragged = _pressed;
	_hovered = -1;
	setToolTip(QString());
	setCursor(Qt::SizeVerCursor);
	update();
}

void AccountsRail::Inner::updateDrag(QPoint position) {
	const auto target = indexAt(position);
	if (target < 0 || target == _dragged || addRow(target)) {
		return;
	}
	const auto entry = _entries[_dragged];
	_entries.erase(_entries.begin() + _dragged);
	_entries.insert(_entries.begin() + target, entry);
	_dragged = target;
	update();
}

void AccountsRail::Inner::finishDrag(QPoint position) {
	_dragged = -1;
	unsetCursor();
	saveOrder();
	updateHovered(position);
}

void AccountsRail::Inner::startDetach() {
	if (_pressed < 0 || addRow(_pressed)) {
		return;
	}
	_detached = _pressed;
	_hovered = -1;
	setToolTip(QString());
	unsetCursor();

	InvokeQueued(this, [weak = base::make_weak(this)] {
		if (!weak) {
			return;
		}
		weak->performDetach();
	});
}

void AccountsRail::Inner::performDetach() {
	if (_detached < 0 || _detached >= int(_entries.size())) {
		return;
	}
	if (!(QApplication::mouseButtons() & Qt::LeftButton)) {
		// The mouse was released before the drag had started.
		_detached = -1;
		_pressed = -1;
		return;
	}
	const auto account = _entries[_detached].account;

	auto mimeData = std::make_unique<QMimeData>();
	mimeData->setData(
		u"application/x-telegram-account"_q,
		QByteArray::number(account->session().uniqueId()));
	mimeData->setText(account->session().user()->name());
	const auto pixmap = Ui::GrabWidget(this, rowRect(_detached));

	// The drag enters the event loop and may destroy this widget,
	// so the weak pointer is created before the drag starts.
	const auto weak = base::make_weak(this);

	// This call enters the event loop and can destroy any QObject.
	_session->window().widget()->launchDrag(
		std::move(mimeData),
		[account, weak] {
			if (!weak) {
				return;
			}
			weak->finishDetach(account, QCursor::pos());
		},
		pixmap);

	// The window may have been destroyed by the drag.
	if (weak) {
		weak->_detached = -1;
		weak->_pressed = -1;
		weak->updateHovered(weak->mapFromGlobal(QCursor::pos()));
	}
}

void AccountsRail::Inner::finishDetach(
		not_null<Main::Account*> account,
		QPoint position) {
	const auto target = windowAt(position);
	if (!target) {
		// Dropped outside of any window, pull the account out.
		detachAccount(account, position);
		crl::on_main(this, [=] { rebuild(); });
		return;
	}
	if (target->id().account == account.get()) {
		// This window already shows the account.
		target->activate();
		return;
	}
	mergeAccount(account, target);
	crl::on_main(this, [=] { rebuild(); });
}

void AccountsRail::Inner::detachAccount(
		not_null<Main::Account*> account,
		QPoint position) {
	// The account can get its own window only if no other window
	// shows it now.
	if (const auto showing = Core::App().separateWindowFor(
			Window::SeparateId(account))) {
		if (!showing->isMain()) {
			// The account already has its own window, move it there.
			showing->activate();
			moveWindow(showing->widget(), position);
			return;
		}
		const auto other = otherAccount(account);
		if (!other) {
			return;
		}
		// This may destroy the current widget.
		showing->showAccount(not_null(other));
	}
	const auto window = Core::App().ensureSeparateWindowFor(
		Window::SeparateId(account));
	moveWindow(window->widget(), position);
}

void AccountsRail::Inner::mergeAccount(
		not_null<Main::Account*> account,
		not_null<Controller*> target) {
	// The account must not be shown in two windows at once.
	if (const auto showing = Core::App().separateWindowFor(
			Window::SeparateId(account))) {
		if (showing->isMain()) {
			// The main window shows another account now, so that the
			// target window is able to show this one.
			const auto other = otherAccount(account);
			if (!other) {
				return;
			}
			// This may destroy the current widget.
			showing->showAccount(not_null(other));
		} else {
			// The account leaves its own window, it is shown
			// in the target window now.
			Core::App().closeWindow(showing);
		}
	}
	Core::App().setActivePrimaryWindow(target);
	const auto domain = &Core::App().domain();
	if (domain->started() && (&domain->active() != account.get())) {
		// The domain shows the account in the active primary window.
		domain->maybeActivate(account);
	} else {
		// The domain does not route the change by itself.
		target->showAccount(account);
	}
}

Main::Account *AccountsRail::Inner::otherAccount(
		not_null<Main::Account*> account) const {
	const auto domain = &Core::App().domain();
	const auto shown = [](not_null<Main::Account*> candidate) {
		return (Core::App().separateWindowFor(
			Window::SeparateId(candidate)) != nullptr);
	};
	if (domain->started()
		&& (&domain->active() != account.get())
		&& !shown(not_null(&domain->active()))) {
		return &domain->active();
	}
	for (const auto &candidate : domain->orderedAccounts()) {
		if (candidate.get() != account.get()
			&& candidate->sessionExists()
			&& !shown(candidate)) {
			return candidate.get();
		}
	}
	return nullptr;
}

void AccountsRail::Inner::moveWindow(
		not_null<::MainWindow*> widget,
		QPoint position) {
	InvokeQueued(widget, [widget, position] {
		const auto margins = widget->frameMargins();
		widget->move(position - QPoint(margins.left(), margins.top()));
	});
}

Controller *AccountsRail::Inner::windowAt(QPoint position) {
	if (const auto widget = QApplication::widgetAt(position)) {
		if (const auto window = Core::App().findWindow(widget)) {
			return window;
		}
	}
	// Fall back to the geometry, the shadow margins are not hit-tested.
	auto result = (Controller*)nullptr;
	Core::App().enumerateWindows([&](not_null<Controller*> window) {
		if (!result && window->widget()->geometry().contains(position)) {
			result = window;
		}
	});
	return result;
}

void AccountsRail::Inner::saveOrder() {
	auto order = std::vector<uint64>();
	order.reserve(_entries.size());
	for (const auto &entry : _entries) {
		order.push_back(entry.account->session().uniqueId());
	}
	// Keep the accounts that are not shown in the rail at the end.
	for (const auto &account : Core::App().domain().orderedAccounts()) {
		if (!account->sessionExists()) {
			order.push_back(account->session().uniqueId());
		}
	}
	Core::App().settings().setAccountsOrder(order);
	Core::App().saveSettings();
}

void AccountsRail::Inner::leaveEventHook(QEvent *e) {
	Ui::RpWidget::leaveEventHook(e);
	if (_hovered >= 0) {
		_hovered = -1;
		setToolTip(QString());
		update();
	}
}

void AccountsRail::Inner::activate(
		int index,
		Qt::KeyboardModifiers modifiers) {
	if (addRow(index)) {
		addAccount();
		return;
	} else if (index < 0 || index >= int(_entries.size())) {
		return;
	}
	const auto account = _entries[index].account;
	if (account.get() == &_session->session().account()) {
		return;
	} else if (modifiers & Qt::ControlModifier) {
		Core::App().ensureSeparateWindowFor(account);
		return;
	}
	if (const auto window = Core::App().separateWindowFor(account)) {
		window->activate();
		return;
	}
	crl::on_main([account] {
		Core::App().domain().maybeActivate(account);
	});
}

void AccountsRail::Inner::addAccount() {
	_session->window().preventOrInvoke([=] {
		Core::App().setActivePrimaryWindow(&_session->window());
		Core::App().domain().addActivated(MTP::Environment{});
	});
}

int AccountsRail::Width() {
	return st::windowAccountsRailWidth;
}

AccountsRail::AccountsRail(
	not_null<Ui::RpWidget*> parent,
	not_null<SessionController*> session)
: Ui::RpWidget(parent)
, _session(session)
, _scroll(this)
, _inner(_scroll.setOwnedWidget(
	object_ptr<Inner>(&_scroll, this))) {
	setAttribute(Qt::WA_OpaquePaintEvent);

	setGeometry(0, 0, Width(), parent->height());
	_scroll.setGeometry(rect());
	_scroll.setFocusPolicy(Qt::NoFocus);

	parent->heightValue(
	) | rpl::on_next([=](int height) {
		setGeometry(0, 0, Width(), height);
		_scroll.setGeometry(rect());
	}, lifetime());

	Core::App().domain().accountsChanges(
	) | rpl::on_next([=] {
		_inner->rebuild();
	}, lifetime());

	_inner->rebuild();
	show();
}

AccountsRail::~AccountsRail() = default;

void AccountsRail::paintEvent(QPaintEvent *e) {
	auto p = Painter(this);
	p.fillRect(rect(), st::windowAccountsRailBg);
}

} // namespace Window
