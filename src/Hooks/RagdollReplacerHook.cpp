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

        auto avOwner = a_this->AsActorValueOwner();
        if (!avOwner)
            return func(a_this, a_unk);

        auto avHealth = avOwner->GetActorValue(RE::ActorValue::kHealth);
	
        // Allow ragdoll if mounted, in midair, dead, or grabbed
        if (a_this->IsOnMount() || a_this->IsInMidair() || avHealth <= 0.05f) {
            return func(a_this, a_unk);
        }

	    auto avGrabbed = avOwner->GetActorValue(RE::ActorValue::kGrabbed);

		// Check if ragdoll replacer is enabled for this actor type
		bool playerReplacer = GetGameSettingBool("bMaxsuPoise_PlayerRagdollReplacer", true);
		bool npcReplacer = GetGameSettingBool("bMaxsuPoise_NPCRagdollReplacer", true);

		bool shouldReplace = (a_this->IsPlayerRef() && playerReplacer) || 
		                     (!a_this->IsPlayerRef() && npcReplacer);

		if (!shouldReplace) {
			return func(a_this, a_unk);
		}

        // Convert ragdoll to custom knockdown animation (for OAR)
        float knockdownDirection = 0.0f;
        a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);

        // Set custom graph variables for OAR conditions
        a_this->SetGraphVariableInt("MaxsuPoise_Knockdown", 1);  // Flag for OAR
        a_this->SetGraphVariableInt("MaxsuPoise_KnockdownDirection",
            (knockdownDirection > 0.25f && knockdownDirection < 0.75f) ? 1 : 0);  // 0=back, 1=forward

        // Flash TrueHUD bar with long duration for dramatic effect
        if (TrueHUDHandler::GetSingleton()->IsEnabled()) {
            TrueHUDHandler::GetSingleton()->FlashPoiseBar(a_this, true);
        }

        // Trigger custom animation event - OAR can replace this based on MaxsuPoise variables
        a_this->NotifyAnimationGraph("MaxsuPoise_KnockdownStart");

        return false;  // Prevent default ragdoll behavior
    }
}
