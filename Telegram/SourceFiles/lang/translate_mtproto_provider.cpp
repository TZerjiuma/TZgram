/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "lang/translate_mtproto_provider.h"

#include "api/api_text_entities.h"
#include "base/debug_log.h"
#include "data/data_peer.h"
#include "data/data_session.h"
#include "main/main_session.h"
#include "mtproto/sender.h"
#include "spellcheck/platform/platform_language.h"

namespace Ui {
namespace {

class MTProtoTranslateProvider final : public TranslateProvider {
public:
	explicit MTProtoTranslateProvider(not_null<Main::Session*> session)
	: _session(session)
	, _api(&session->mtp()) {
	}

	[[nodiscard]] bool supportsMessageId() const override {
		return true;
	}

	void request(
			TranslateProviderRequest request,
			LanguageId to,
			Fn<void(TranslateProviderResult)> done) override {
		requestBatch(
			{ std::move(request) },
			to,
			[done = std::move(done)](
					int,
					TranslateProviderResult result) {
				done(std::move(result));
			},
			[] {});
	}

	void requestBatch(
			std::vector<TranslateProviderRequest> requests,
			const LanguageId &to,
			Fn<void(int, TranslateProviderResult)> doneOne,
			Fn<void()> doneAll) override {
		using Flag = MTPmessages_TranslateText::Flag;
		if (requests.empty()) {
			doneAll();
			return;
		}

		const auto failAll = [=] {
			for (auto i = 0; i != requests.size(); ++i) {
				doneOne(i, TranslateProviderResult{
					.error = TranslateProviderError::Unknown,
				});
			}
			doneAll();
		};
		const auto doneFromList = [=, session = _session](
				const QVector<MTPTextWithEntities> &list) {
			for (auto i = 0; i != requests.size(); ++i) {
				doneOne(
					i,
					(i < list.size())
						? TranslateProviderResult{
							.text = Api::ParseTextWithEntities(
								session,
								list[i]),
						}
						: TranslateProviderResult{
							.error = TranslateProviderError::Unknown,
						});
			}
			doneAll();
		};

		const auto firstPeer = PeerId(requests.front().peerId);
		const auto allWithIds = ranges::all_of(
			requests,
			[&](const TranslateProviderRequest &request) {
				return (PeerId(request.peerId) == firstPeer)
					&& (request.msgId != 0);
			});
		if (allWithIds) {
			const auto peer = _session->data().peerLoaded(firstPeer);
			if (!peer) {
				LOG(("Translate: peer %1 is not loaded")
					.arg(firstPeer.value));
				failAll();
				return;
			}
			auto ids = QVector<MTPint>();
			ids.reserve(requests.size());
			for (const auto &request : requests) {
				ids.push_back(MTP_int(MsgId(request.msgId)));
			}
			LOG(("Translate: request by peer+id, peer=%1, count=%2")
				.arg(peer->id.value)
				.arg(requests.size()));
			_api.request(MTPmessages_TranslateText(
				MTP_flags(Flag::f_peer | Flag::f_id),
				peer->input(),
				MTP_vector<MTPint>(ids),
				MTPVector<MTPTextWithEntities>(),
				MTP_string(to.twoLetterCode()),
				MTPstring() // tone
			)).done([=](const MTPmessages_TranslatedText &result) {
				const auto &list = result.data().vresult().v;
				LOG(("Translate: peer+id returned %1 results")
					.arg(list.size()));
				doneFromList(list);
			}).fail([=](const MTP::Error &error) {
				LOG(("Translate: peer+id failed, code=%1, type=%2, "
					"description=%3")
					.arg(error.code())
					.arg(error.type())
					.arg(error.description()));
				failAll();
			}).send();
			return;
		}

		const auto allWithText = ranges::all_of(
			requests,
			[](const TranslateProviderRequest &request) {
				return !request.text.text.isEmpty();
			});
		if (!allWithText) {
			// Neither a message id nor a text to send: fail instead of
			// recursing into the base class implementation of requestBatch().
			LOG(("Translate: request has neither a message id nor a text"));
			failAll();
			return;
		}

		auto text = QVector<MTPTextWithEntities>();
		text.reserve(requests.size());
		for (const auto &request : requests) {
			text.push_back(MTP_textWithEntities(
				MTP_string(request.text.text),
				Api::EntitiesToMTP(
					_session,
					request.text.entities,
					Api::ConvertOption::SkipLocal)));
		}
		LOG(("Translate: request by text, count=%1").arg(requests.size()));
		_api.request(MTPmessages_TranslateText(
			MTP_flags(Flag::f_text),
			MTP_inputPeerEmpty(),
			MTPVector<MTPint>(),
			MTP_vector<MTPTextWithEntities>(text),
			MTP_string(to.twoLetterCode()),
			MTPstring() // tone
		)).done([=](const MTPmessages_TranslatedText &result) {
			const auto &list = result.data().vresult().v;
			LOG(("Translate: text returned %1 results").arg(list.size()));
			doneFromList(list);
		}).fail([=](const MTP::Error &error) {
			LOG(("Translate: text failed, code=%1, type=%2, description=%3")
				.arg(error.code())
				.arg(error.type())
				.arg(error.description()));
			failAll();
		}).send();
	}

private:
	const not_null<Main::Session*> _session;
	MTP::Sender _api;

};

} // namespace

std::unique_ptr<TranslateProvider> CreateMTProtoTranslateProvider(
		not_null<Main::Session*> session) {
	return std::make_unique<MTProtoTranslateProvider>(session);
}

} // namespace Ui
