#pragma once
#include "DKUtil/Config.hpp"

namespace MaxsuPoise
{
	using WEAPON_TYPE = RE::WEAPON_TYPE;
	using BipedSlot = RE::BGSBipedObjectForm::BipedObjectSlot;
	using EventResult = RE::BSEventNotifyControl;

	class SettingsHandler : public RE::BSTEventSink<SKSE::ModCallbackEvent>
	{
	public:
		static inline std::map<WEAPON_TYPE, float> weapTypeMultMap;
		static inline std::map<std::string, float> weapKeywordMultMap;
		static inline std::map<BipedSlot, float> armorSlotMultMap;
		static inline std::map<std::string, float> creatureRaceMultMap;
		static inline std::map<std::string, float> creaturePoiseMultMap;
		static inline std::map<std::string, std::string> gameSettingsMap;

		static constexpr char modName[] = "MaxsuPoise";
		static bool Register();

		virtual EventResult ProcessEvent(const SKSE::ModCallbackEvent* a_event, RE::BSTEventSource<SKSE::ModCallbackEvent>*)
		{
			if (a_event && _strcmpi(a_event->eventName.c_str(), "dmenu_updateSettings") == 0 && _strcmpi(a_event->strArg.c_str(), modName) == 0) {
				UpdateWeapTypeMult();
			}

			return EventResult::kContinue;
		}

	private:
		SettingsHandler() = default;

		static void UpdateWeapTypeMult();
		static void InitWeapKeywordMult();
		static void InitArmorSlotMult();
		static void InitCreatureRaceMult();
		static void InitCreaturePoiseMult();
		static void InitGameSettings();
	};

}