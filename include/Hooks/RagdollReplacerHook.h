#pragma once

namespace MaxsuPoise
{
	class RagdollReplacerHook
	{
	public:
		static void InstallHooks();

	private:
		class Hooks
		{
		public:
			struct IsActorKnockdown
			{
				static bool thunk(RE::Character* a_this, std::int64_t a_unk);
				static inline REL::Relocation<decltype(thunk)> func;
			};

			static void Install()
			{
				stl::write_thunk_call<IsActorKnockdown>(REL::RelocationID(38858, 39895).address() + REL::Relocate(0x7E, 0x68));
				INFO("IsActorKnockdown hook installed");
			}
		};
	};
}
