// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HopperCharacterInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UHopperCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Character functions abstracted through interface calls
 */
class HOPPER_API IHopperCharacterInterface
{
	GENERATED_BODY()

public:
	virtual void ApplyPunchForceToCharacter(const FVector FromLocation, const float InAttackForce) const = 0;
};
