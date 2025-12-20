#include "Hooks/RagdollReplacerHook.h"
#include "TrueHUDHandler.h"
#include "Utils.h"

namespace MaxsuPoise
{
	void RagdollReplacerHook::InstallHooks()
	{
		Hooks::Install();
	}

	bool RagdollReplacerHook::Hooks::IsActorKnockdown::thunk(RE::Character* a_this, std::int64_t a_unk)
	{
		if (!a_this)
			return func(a_this, a_unk);

		auto avHealth = a_this->GetActorValue(RE::ActorValue::kHealth);
		
		// Allow ragdoll if mounted, in midair, dead, or grabbed
		if (a_this->IsOnMount() || a_this->IsInMidair() || avHealth <= 0.05f) {
			return func(a_this, a_unk);
		}

		auto avGrabbed = a_this->GetActorValue(RE::ActorValue::kGrabbed);
		if (avGrabbed >= 1.0f) {
			return func(a_this, a_unk);
		}

		// Check if ragdoll replacer is enabled for this actor type
		bool playerReplacer = GetGameSettingBool("bMaxsuPoise_PlayerRagdollReplacer", true);
		bool npcReplacer = GetGameSettingBool("bMaxsuPoise_NPCRagdollReplacer", true);

		bool shouldReplace = (a_this->IsPlayerRef() && playerReplacer) || 
		                     (!a_this->IsPlayerRef() && npcReplacer);

		if (!shouldReplace) {
			return func(a_this, a_unk);
		}

        // Convert ragdoll to knockdown animation (via OAR)
        float knockdownDirection = 0.0f;
        a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);

        // Set direction for OAR conditions
        a_this->SetGraphVariableFloat("Direction", knockdownDirection);
        
        // Trigger custom event for OAR to replace with knockdown animations
        RE::BSFixedString knockdownEvent = "MaxsuPoise_Knockdown";
        
        // Flash TrueHUD bar with long duration for dramatic effect
        if (TrueHUDHandler::GetSingleton()->IsEnabled()) {
            TrueHUDHandler::GetSingleton()->FlashPoiseBar(a_this, true);
        }

        a_this->NotifyAnimationGraph(knockdownEvent);
	}
}
