#include "LoadGame.h"
#include "Hooks/HitEventHandler.h"
#include "Hooks/MagicStaggerHook.h"
#include "Hooks/PerkEntry_Hooks.h"
#include "Hooks/PoiseRegenHandler.h"
#include "SettingsHandler.h"
#include "TrueHUDHandler.h"

namespace MaxsuPoise
{
	void EventCallback(SKSE::MessagingInterface::Message* msg)
	{
		if (msg->type == SKSE::MessagingInterface::kPostPostLoad) {
			static constexpr auto bdiDLLName = "BehaviorDataInjector.dll";
			auto bdiPlugin = GetModuleHandleA(bdiDLLName);
			if (!bdiPlugin) {
				ERROR("Not \"{}\" Loaded!", bdiDLLName);
				return;
			}

			MaxsuPoise::SettingsHandler::Register();
			MaxsuPoise::HitEventHandler::InstallHooks();
			MaxsuPoise::MagicStaggerHook::InstallHooks();
			MaxsuPoise::PoiseRegenHandler::CharacterEx::InstallHook();
			MaxsuPoise::PoiseRegenHandler::PlayerEx::InstallHook();
			MaxsuPoise::PerkEntryHook::Install();
		}
		else if (msg->type == SKSE::MessagingInterface::kDataLoaded) {
			MaxsuPoise::TrueHUDHandler::GetSingleton()->Initialize();
		}
		else if (msg->type == SKSE::MessagingInterface::kPreLoadGame) {
			// Release TrueHUD control before loading a new game
			MaxsuPoise::TrueHUDHandler::GetSingleton()->Shutdown();
		}
		else if (msg->type == SKSE::MessagingInterface::kPostLoadGame) {
			// Initialize TrueHUD AFTER save loads (so we can override saved control)
			MaxsuPoise::TrueHUDHandler::GetSingleton()->Initialize();
		}
	}

}
