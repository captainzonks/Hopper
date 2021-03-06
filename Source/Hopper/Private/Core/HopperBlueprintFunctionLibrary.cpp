// © 2021, Matthew Barham. All rights reserved.


#include "Core/HopperBlueprintFunctionLibrary.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

bool UHopperBlueprintFunctionLibrary::IsSquashingHit(const FVector Velocity, const float VelocityToKill)
{
	if ((Velocity.IsNormalized() ? Velocity.Z : Velocity.GetSafeNormal().Z) > VelocityToKill)
		return true;
	return false;
}

void UHopperBlueprintFunctionLibrary::ApplyPunchForceToCharacter(const FVector FromLocation, const ACharacter* Target,
                                                                 const float AttackForce)
{
	if (IsValid(Target))
	{
		const FVector TargetLocation = Target->GetActorLocation();
		const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation, TargetLocation);

		Target->GetCharacterMovement()->Launch(FVector(
			Direction.X * AttackForce,
			Direction.Y * AttackForce,
			abs(Direction.Z + 1) * AttackForce));
	}
}
