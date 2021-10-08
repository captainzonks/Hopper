// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/HopperBaseCharacter.h"
#include "HopperEnemy.generated.h"

/**
 * Hopper Enemy class w/ Squash functions
 */
UCLASS()
class HOPPER_API AHopperEnemy : public AHopperBaseCharacter
{
	GENERATED_BODY()

public:
	AHopperEnemy();

protected:
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
	 * Compresses the sprite and shifts it downward to make the character appear "squashed"
	 * then sets a timer and calls Event OnSquashed after the provided time runs out.
	 * @param TimeTillDestroy Time to hold the squashed state before destroying the actor.
	 */
	UFUNCTION(BlueprintCallable)
	void Squash(const float TimeTillDestroy = 1.f);
	
	FTimerHandle DeathHandle;
};
