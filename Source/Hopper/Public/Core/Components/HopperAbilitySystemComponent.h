// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "Core/Hopper.h"
#include "AbilitySystemComponent.h"
#include "HopperAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOPPER_API UHopperAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

	UHopperAbilitySystemComponent();
	
	/** Version of function in AbilitySystemGlobals that returns correct type */
	static UHopperAbilitySystemComponent* GetAbilitySystemComponentFromActor(const AActor* Actor, bool LookForComponent = false);
	
};
