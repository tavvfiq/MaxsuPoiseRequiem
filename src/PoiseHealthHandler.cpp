#include "PoiseHealthHandler.h"
#include "SettingsHandler.h"
#include "Utils.h"

namespace MaxsuPoise
{
	static constexpr char CURRENT_POISE_HEALTH_GV[] = "MaxsuPoise_PoiseHealth";

	float PoiseHealthHandler::GetCurrentPoiseHealth(RE::Actor* a_target)
	{
		float result = 0.f;
		if (!a_target || !a_target->GetGraphVariableFloat(CURRENT_POISE_HEALTH_GV, result))
			WARN("Not Graph Variable Float Found: {}", CURRENT_POISE_HEALTH_GV);
		
		// Guard against blocking animation glitch: if graph variable returns 0 but actor is valid,
		// return total poise health instead to prevent phantom bar flashing
		if (result == 0.f && a_target && a_target->Is3DLoaded()) {
			return GetTotalPoiseHealth(a_target);
		}

		return result;
	}

	bool PoiseHealthHandler::SetCurrentPoiseHealth(RE::Actor* a_target, const float& a_in)
	{
		return a_target && a_target->SetGraphVariableFloat(CURRENT_POISE_HEALTH_GV, a_in);
	}

	float PoiseHealthHandler::GetTotalPoiseHealth(RE::Actor* a_target)
	{
		if (!a_target || !a_target->Is3DLoaded() || !a_target->IsInitialized())
			return 0.f;

		auto actorPoiseHealth = GetBasePoiseHealth() * GetActorMass(a_target) * a_target->GetScale();
		auto armorPoiseHealth = GetTotalArmorPoiseHealth(a_target);
		auto armorRatingBonus = GetArmorRatingBonus(a_target);
		auto staminaPenalty = GetStaminaPenalty(a_target);
		auto creaturePoiseHealthMult = GetCreaturePoiseHealthMult(a_target);
		
		return (actorPoiseHealth + armorPoiseHealth + armorRatingBonus) * staminaPenalty * creaturePoiseHealthMult;
	}

	float PoiseHealthHandler::GetBasePoiseHealth()
	{
		return GetGameSettingFloat("fMaxsuPoise_BasePoiseHealth", 40.f);
	}

	float PoiseHealthHandler::GetTotalArmorPoiseHealth(RE::Actor* a_target)
	{
		float result = 0.f;

		if (!a_target)
			return result;

		auto baseArmorPoiseHealth = GetBaseArmorPoiseHealth();
		auto heavyArmorBouns = GetHeavyArmorBouns();

		for (const auto& pair : SettingsHandler::armorSlotMultMap) {
			auto slot = pair.first;
			auto armor = a_target->GetWornArmor(slot);
			auto armorHealth = baseArmorPoiseHealth * pair.second;
			if (armor && (armor->IsLightArmor() || armor->IsHeavyArmor())) {
				result += armor->IsLightArmor() ? armorHealth : armorHealth * (1 + heavyArmorBouns);
			}
			else {
				armor = a_target->GetSkin(slot);
				if (armor && (armor->IsLightArmor() || armor->IsHeavyArmor()))
					result += armor->IsLightArmor() ? armorHealth : armorHealth * (1 + heavyArmorBouns);
			}
		}

		return result;
	}

	float PoiseHealthHandler::GetArmorRatingBonus(RE::Actor* a_target)
	{
		if (!a_target)
			return 0.f;

		auto armorRating = a_target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kDamageResist);
		auto armorScale = GetGameSettingFloat("fMaxsuPoise_ArmorRatingScale", 0.5f);
		
		return armorRating * armorScale;
	}

	float PoiseHealthHandler::GetStaminaPenalty(RE::Actor* a_target)
	{
		if (!a_target)
			return 1.0f;

		auto race = a_target->GetRace();
		if (race && !race->AllowsPCDialogue())
			return 1.0f;

		auto currentStamina = a_target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kStamina);
		auto maxStamina = a_target->AsActorValueOwner()->GetPermanentActorValue(RE::ActorValue::kStamina);
		
		if (maxStamina <= 0.f)
			return 1.0f;

		auto staminaPercent = currentStamina / maxStamina;
		auto minPenalty = GetGameSettingFloat("fMaxsuPoise_MinStaminaMult", 0.5f);
		
		return std::max(minPenalty, staminaPercent);
	}

	float PoiseHealthHandler::GetBaseArmorPoiseHealth()
	{
		return GetGameSettingFloat("fMaxsuPoise_BaseArmorPoiseHealth", 50.f);
	}

	float PoiseHealthHandler::GetHeavyArmorBouns()
	{
		return GetGameSettingFloat("fMaxsuPoise_HeavyArmorPoiseBonus", 0.5f);
	}

	float PoiseHealthHandler::GetCreaturePoiseHealthMult(RE::Actor* a_target)
	{
		if (!a_target)
			return 1.0f;

		auto race = a_target->GetRace();
		if (!race)
			return 1.0f;

		// Humanoid races use default 1.0x
		if (race->AllowsPCDialogue())
			return 1.0f;

		auto raceName = race->GetFormEditorID();
		if (!raceName || strlen(raceName) == 0)
			return 1.0f;

		// Check creature poise multiplier map
		for (const auto& [raceKeyword, mult] : SettingsHandler::creaturePoiseMultMap) {
			if (_strnicmp(raceName, raceKeyword.c_str(), raceKeyword.length()) == 0) {
				return mult;
			}
		}

		// No specific multiplier found, use default 1.0
		return 1.0f;
	}
}