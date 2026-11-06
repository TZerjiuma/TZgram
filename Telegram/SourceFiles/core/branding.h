/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#pragma once

#include "core/version.h"
#include "lang/lang_keys.h"
#include "ui/style/style_core_scale.h"
#include "ui/text/custom_emoji_helper.h"
#include "ui/text/text_entity.h"
#include "ui/text/text_utilities.h"
#include "styles/style_basic.h"

#include <QtGui/QImage>

namespace Branding {

struct Data {
	TextWithEntities text;
	Ui::Text::MarkedContext context;
};

[[nodiscard]] inline const Data &Get() {
	// The images must be registered before the context is created,
	// otherwise it has no custom emoji factory and the placeholder text
	// is shown instead of the images.
	static const auto result = [] {
		auto helper = Ui::Text::CustomEmojiHelper();

		auto text = tr::marked(AppDeveloperText.utf16());
		// The logo and the source code link are disabled for now: both of
		// them point to nowhere, so they only confuse the user.
		// text.append(QChar('\n'));
		// const auto logo = Icon(u":/gui/art/logo_256.png"_q);
		// if (!logo.isNull()) {
		// 	text.append(Ui::Text::Link(
		// 		helper.image({
		// 			.image = logo,
		// 			.textColor = false,
		// 		}),
		// 		AppChannelUrl.utf16()));
		// 	text.append(QChar(' '));
		// }
		// const auto repo = Icon(u":/gui/art/github.png"_q);
		// if (!repo.isNull()) {
		// 	text.append(Ui::Text::Link(
		// 		helper.image({
		// 			.image = repo,
		// 			.textColor = true,
		// 		}),
		// 		AppRepoUrl.utf16()));
		// }

		return Data{
			.text = std::move(text),
			.context = helper.context(),
		};
	}();
	return result;
}

[[nodiscard]] inline const TextWithEntities &Text() {
	return Get().text;
}

[[nodiscard]] inline Ui::Text::MarkedContext Context() {
	return Get().context;
}

[[nodiscard]] inline QString Title() {
	return AppName.utf16() + QChar(' ') + AppDesktopSuffix.utf16();
}

} // namespace Branding
