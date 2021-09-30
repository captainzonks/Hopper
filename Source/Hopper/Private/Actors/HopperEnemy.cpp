// © 2021, Matthew Barham. All rights reserved.


#include "Actors/HopperEnemy.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"

AHopperEnemy::AHopperEnemy()
{
	SquashPoint = CreateDefaultSubobject<USceneComponent>(TEXT("Squash Point"));
	SquashPoint->SetupAttachment(RootComponent);
	SquashPoint->SetRelativeLocation(FVector(0.f, 0.f, 70.f));

	DeathCallback.BindLambda([this](){OnSquashed();});
}

bool AHopperEnemy::IsSquashingHit(const FVector InImpactPoint, const float ErrorTolerance) const
{
	if (InImpactPoint.Equals(SquashPoint->GetComponentLocation(), ErrorTolerance))
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
			AIController->StopMovement();
			AIController->GetBrainComponent()->StopLogic(FString("AI has been squashed."));
		}
	}

	FVector NewScale{GetSprite()->GetRelativeScale3D()};
	NewScale.Z /= 2.f;

	GetSprite()->SetRelativeScale3D(NewScale);
	GetSprite()->AddLocalOffset(FVector(0.f, 0.f, -55.f), false, nullptr, ETeleportType::TeleportPhysics);

	GetWorldTimerManager().SetTimer(DeathHandle, DeathCallback, TimeTillDestroy, false);
}
