#pragma once

#include "TrueHUDAPI.h"

namespace MaxsuPoise
{
	class TrueHUDHandler
	{
	public:
		static TrueHUDHandler* GetSingleton();

		void Initialize();
		bool IsEnabled() const { return g_trueHUD != nullptr; }

		void FlashPoiseBar(RE::Actor* a_actor, bool a_long = false);

	private:
		TrueHUDHandler() = default;
		~TrueHUDHandler() = default;

		TrueHUDHandler(const TrueHUDHandler&) = delete;
		TrueHUDHandler(TrueHUDHandler&&) = delete;
		TrueHUDHandler& operator=(const TrueHUDHandler&) = delete;
		TrueHUDHandler& operator=(TrueHUDHandler&&) = delete;

		static float GetCurrentPoiseResource(RE::Actor* a_actor);
		static float GetMaxPoiseResource(RE::Actor* a_actor);

		TRUEHUD_API::IVTrueHUD4* g_trueHUD = nullptr;
		SKSE::PluginHandle g_pluginHandle = SKSE::kInvalidPluginHandle;
	};
}
