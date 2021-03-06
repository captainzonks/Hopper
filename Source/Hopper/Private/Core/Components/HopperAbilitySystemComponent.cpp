// © 2021, Matthew Barham. All rights reserved.


#include "Core/Components/HopperAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"

UHopperAbilitySystemComponent::UHopperAbilitySystemComponent()
{
}

UHopperAbilitySystemComponent* UHopperAbilitySystemComponent::GetAbilitySystemComponentFromActor(const AActor* Actor,
	bool LookForComponent)
{
	return Cast<UHopperAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor, LookForComponent));
}
