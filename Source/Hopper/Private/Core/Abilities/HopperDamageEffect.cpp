// © 2021, Matthew Barham. All rights reserved.


#include "Core/Abilities/HopperDamageEffect.h"

#include "GameplayTagsManager.h"
#include "Core/Abilities/HopperAttributeSet.h"

UHopperDamageEffect::UHopperDamageEffect()
{
	FGameplayModifierInfo DamageInfo;
	DamageInfo.Attribute = UHopperAttributeSet::GetDamageAttribute();
	DamageInfo.ModifierOp = EGameplayModOp::Override;
	DamageInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(35.f);
	Modifiers.Add(DamageInfo);

	FGameplayEffectCue DamageCue;
	FGameplayTagContainer DamageTagContainer;
	TArray<FString> Tags;
	Tags.Add("GameplayCue.Punched");
	UGameplayTagsManager::Get().RequestGameplayTagContainer(Tags, DamageTagContainer);
	DamageCue.GameplayCueTags = DamageTagContainer;
	GameplayCues.Add(DamageCue);
}
