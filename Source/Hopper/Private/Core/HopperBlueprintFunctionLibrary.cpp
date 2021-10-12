// © 2021, Matthew Barham. All rights reserved.


#include "Core/HopperBlueprintFunctionLibrary.h"

bool UHopperBlueprintFunctionLibrary::IsSquashingHit(const FVector Velocity, const float VelocityToKill)
{
	if ((Velocity.IsNormalized() ? Velocity.Z : Velocity.GetSafeNormal().Z) > VelocityToKill)
		return true;
	return false;
}
