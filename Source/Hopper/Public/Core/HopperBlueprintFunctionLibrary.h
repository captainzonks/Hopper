// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "HopperBlueprintFunctionLibrary.generated.h"

/**
 * Hopper Helper Functions
 */
UCLASS()
class HOPPER_API UHopperBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Compares the provided Velocity's Z value with the provided VelocityToKill float.
	 * Function normalizes the Velocity if it isn't already.
	 * @param Velocity Velocity of Actor which hit
	 * @param VelocityToKill Z-Velocity required to cause a squash
	 * @return True if impact counts as a squash.
	 */
	UFUNCTION(BlueprintPure)
	static bool IsSquashingHit(const FVector Velocity, const float VelocityToKill = 0.f);

	/**
	 * Launches Target away from the provided FromLocation using the provided AttackForce.
	 * @param FromLocation Location of attacker or cause of launch
	 * @param Target Character to be launched
	 * @param AttackForce Force applied to the launch
	 */
	UFUNCTION(BlueprintCallable)
	static void ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target,
	                                       const float AttackForce);
};
