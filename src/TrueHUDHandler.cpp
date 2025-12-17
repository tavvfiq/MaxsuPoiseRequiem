#include "TrueHUDHandler.h"
#include "PoiseHealthHandler.h"

namespace MaxsuPoise
{
	TrueHUDHandler* TrueHUDHandler::GetSingleton()
	{
		static TrueHUDHandler singleton;
		return &singleton;
	}

	void TrueHUDHandler::Initialize()
	{
		INFO("Initializing TrueHUD integration...");

		g_pluginHandle = SKSE::GetPluginHandle();

		g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD4*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V4));
		if (!g_trueHUD) {
			g_trueHUD = reinterpret_cast<TRUEHUD_API::IVTrueHUD4*>(TRUEHUD_API::RequestPluginAPI(TRUEHUD_API::InterfaceVersion::V3));
		}

		if (g_trueHUD) {
			INFO("TrueHUD API acquired successfully");

			auto result = g_trueHUD->RequestSpecialResourceBarsControl(g_pluginHandle);
			if (result == TRUEHUD_API::APIResult::OK) {
				INFO("Special resource bar control acquired");

				result = g_trueHUD->RegisterSpecialResourceFunctions(
					g_pluginHandle,
					GetCurrentPoiseResource,
					GetMaxPoiseResource,
					false,
					true);

				if (result == TRUEHUD_API::APIResult::OK) {
					INFO("TrueHUD integration initialized successfully - Poise bar registered");
				} else {
					WARN("Failed to register special resource functions");
				}
			} else if (result == TRUEHUD_API::APIResult::AlreadyTaken) {
				WARN("Special resource bar control is already taken by another plugin");
			} else {
				WARN("Failed to acquire special resource bar control");
			}
		} else {
			INFO("TrueHUD not detected - skipping integration");
		}
	}

	void TrueHUDHandler::FlashPoiseBar(RE::Actor* a_actor, bool a_long)
	{
		if (!g_trueHUD || !a_actor) {
			return;
		}

		RE::ActorHandle handle = a_actor->GetHandle();
		g_trueHUD->FlashActorSpecialBar(g_pluginHandle, handle, a_long);
	}

	float TrueHUDHandler::GetCurrentPoiseResource(RE::Actor* a_actor)
	{
		if (!a_actor) {
			return 0.0f;
		}

		return PoiseHealthHandler::GetCurrentPoiseHealth(a_actor);
	}

	float TrueHUDHandler::GetMaxPoiseResource(RE::Actor* a_actor)
	{
		if (!a_actor) {
			return 100.0f;
		}

		auto maxPoise = PoiseHealthHandler::GetTotalPoiseHealth(a_actor);
		return maxPoise > 0.0f ? maxPoise : 100.0f;
	}
}
