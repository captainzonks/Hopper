// © 2021, Matthew Barham. All rights reserved.


#include "Actors/HopperEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "PaperFlipbookComponent.h"
#include "Actors/HopperPlayerCharacter.h"
#include "Components/CapsuleComponent.h"

AHopperEnemy::AHopperEnemy()
{
}

bool AHopperEnemy::IsSquashingHit(const FVector Velocity, const float VelocityToKill)
{
	if ((Velocity.IsNormalized() ? Velocity.Z : Velocity.GetSafeNormal().Z) > VelocityToKill)
		return true;
	return false;
}

void AHopperEnemy::Squash(const float TimeTillDestroy)
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UE_LOG(LogTemp, Warning, TEXT("Squashed!"))

	if (IsValid(Controller))
	{
		const auto AIController = Cast<AAIController>(Controller);
		if (IsValid(AIController))
		{
			const auto Brain = AIController->GetBrainComponent();
			if (IsValid(Brain))
			{
				AIController->StopMovement();
				AIController->GetBrainComponent()->StopLogic(FString("AI has been squashed"));
			}
		}
	}

	FVector NewScale{GetSprite()->GetRelativeScale3D()};
	NewScale.Z /= 2.f;

	GetSprite()->SetRelativeScale3D(NewScale);
	GetSprite()->AddLocalOffset(FVector(0.f, 0.f, -55.f), false, nullptr, ETeleportType::TeleportPhysics);

	GetWorldTimerManager().SetTimer(DeathHandle, this, &AHopperEnemy::CharacterDeath, TimeTillDestroy, false);
}
