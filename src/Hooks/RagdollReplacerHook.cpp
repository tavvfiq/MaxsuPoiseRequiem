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

        // Convert ragdoll to largest stagger animation based on direction
        float knockdownDirection = 0.0f;
        a_this->GetGraphVariableFloat("staggerDirection", knockdownDirection);

        // Set direction for MSL (0 = backward, 1 = forward)
        a_this->SetGraphVariableInt("msl_staggerDirection", 
            (knockdownDirection > 0.25f && knockdownDirection < 0.75f) ? 1 : 0);
        
        // Set largest stagger level for MSL
        a_this->SetGraphVariableInt("msl_staggerLevel", 4);
        
        // Flash TrueHUD bar with long duration for dramatic effect
        if (TrueHUDHandler::GetSingleton()->IsEnabled()) {
            TrueHUDHandler::GetSingleton()->FlashPoiseBar(a_this, true);
        }

        // Trigger stagger - MSL will route to largest tier based on msl_staggerLevel
        a_this->NotifyAnimationGraph("StaggerStart");

        return false;  // Prevent ragdoll
    }
}
