// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/HopperData.h"
#include "Abilities/GameplayAbility.h"
#include "HopperGameplayAbility.generated.h"

/**
 * Hopper Gameplay Ability
 */
UCLASS()
class HOPPER_API UHopperGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UHopperGameplayAbility();

	// Abilities will activate when input is pressed
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ability")
	EHopperAbilityInputID AbilityInputID {EHopperAbilityInputID::None};
};
