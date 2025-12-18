#include "Utils.h"
#include "SettingsHandler.h"

namespace MaxsuPoise
{

	float GetGameSettingFloat(const std::string a_name, const float a_default)
	{
		auto it = SettingsHandler::gameSettingsMap.find(a_name);
		if (it != SettingsHandler::gameSettingsMap.end()) {
			try {
				return std::stof(it->second);
			} catch (...) {
				ERROR("Failed to parse float from setting: {} = {}", a_name, it->second);
				return a_default;
			}
		}
		return a_default;
	}

	std::uint32_t GetGameSettingUInt(const std::string a_name, const std::uint32_t a_default)
	{
		auto it = SettingsHandler::gameSettingsMap.find(a_name);
		if (it != SettingsHandler::gameSettingsMap.end()) {
			try {
				return static_cast<std::uint32_t>(std::stoul(it->second));
			} catch (...) {
				ERROR("Failed to parse uint from setting: {} = {}", a_name, it->second);
				return a_default;
			}
		}
		return a_default;
	}

	bool GetGameSettingBool(const std::string a_name, const bool a_default)
	{
		auto it = SettingsHandler::gameSettingsMap.find(a_name);
		if (it != SettingsHandler::gameSettingsMap.end()) {
			try {
				std::string value = it->second;
				// Convert to lowercase for case-insensitive comparison
				std::transform(value.begin(), value.end(), value.begin(), ::tolower);
				if (value == "true" || value == "1") {
					return true;
				} else if (value == "false" || value == "0") {
					return false;
				}
				return std::stoi(it->second) != 0;
			} catch (...) {
				ERROR("Failed to parse bool from setting: {} = {}", a_name, it->second);
				return a_default;
			}
		}
		return a_default;
	}

	float GetActorMass(RE::Actor* a_target)
	{
		if (!a_target || !a_target->GetActorRuntimeData().race)
			return 0.f;

		return a_target->AsActorValueOwner()->GetActorValue(RE::ActorValue::kMass);
	}

	void CPrint(const char* a_fmt, ...)
	{
		auto console = RE::ConsoleLog::GetSingleton();
		if (console) {
			std::va_list args;
			va_start(args, a_fmt);
			console->VPrint(a_fmt, args);
			va_end(args);
		}
	}

	void ApplyPerkEntryPoint(INT32 entry, RE::Actor* actor_a, RE::Actor* actor_b, float* out)
	{
		using func_t = decltype(&ApplyPerkEntryPoint);
		static REL::Relocation<func_t> func{ REL::RelocationID(23073, 23526) };  // 1.5.97 14032ECE0
		return func(entry, actor_a, actor_b, out);
	}

	RE::hkbClipGenerator* ToClipGenerator(RE::hkbNode* a_node)
	{
		constexpr char CLASS_NAME[] = "hkbClipGenerator";

		if (a_node && a_node->GetClassType()) {
			if (_strcmpi(a_node->GetClassType()->name, CLASS_NAME) == 0)
				return skyrim_cast<RE::hkbClipGenerator*>(a_node);
		}

		return nullptr;
	}

}
