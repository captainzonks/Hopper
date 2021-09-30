// © 2021, Matthew Barham. All rights reserved.

#include "Actors/HopperBaseCharacter.h"

#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

AHopperBaseCharacter::AHopperBaseCharacter()
{
	OnCharacterMovementUpdated.AddDynamic(this, &AHopperBaseCharacter::Animate);

	GetCharacterMovement()->GravityScale = 2.8f;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];

	bReplicates = true;

	GetCapsuleComponent()->SetCapsuleRadius(70.f);

	GetSprite()->SetRelativeScale3D(FVector(11.f, 11.f, 11.f));
	GetSprite()->SetUsingAbsoluteRotation(true);
	GetSprite()->SetFlipbook(Flipbooks.IdleDown);
	GetSprite()->CastShadow = true;
}

void AHopperBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
}

void AHopperBaseCharacter::OnJumped_Implementation()
{
	GetCharacterMovement()->bNotifyApex = true;

	JumpCounter++;
	GetWorldTimerManager().ClearTimer(JumpReset);

	Super::OnJumped_Implementation();
}

void AHopperBaseCharacter::Landed(const FHitResult& Hit)
{
	GetCharacterMovement()->GravityScale = 2.8f;

	if (JumpCounter > 2)
		ResetJumpPower();

	ModifyJumpPower();
	GetWorldTimerManager().SetTimer(JumpReset, this, &AHopperBaseCharacter::ResetJumpPower, 0.2f, false);
	
	Super::Landed(Hit);
}

void AHopperBaseCharacter::NotifyJumpApex()
{
	GetCharacterMovement()->GravityScale = 5.f;
	Super::NotifyJumpApex();
}

void AHopperBaseCharacter::ModifyJumpPower()
{
	switch (JumpCounter)
	{
	case 1:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[1];
		UE_LOG(LogTemp, Warning, TEXT("Jump Power Level 1"))
		break;
	case 2:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[2];
		UE_LOG(LogTemp, Warning, TEXT("Jump Power Level 2"))
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("Jump Power Level 0"))
		break;
	}
}

void AHopperBaseCharacter::ResetJumpPower()
{
	JumpCounter = 0;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];
	UE_LOG(LogTemp, Warning, TEXT("Jump Power Reset"))
}


void AHopperBaseCharacter::Animate(float DeltaTime, FVector OldLocation, const FVector OldVelocity)
{
	TOptional<FMinimalViewInfo> ViewInfo;
	if (!IsPlayerControlled())
	{
		UWorld* World = GetWorld();
		if (World)
		{
			APlayerController* PlayerController = World->GetFirstPlayerController();
			if (PlayerController)
			{
				ACharacter* Character = PlayerController->GetCharacter();
				if (Character)
				{
					Character->CalcCamera(DeltaTime, ViewInfo.Emplace());
				}
			}
		}
	}

	SetCurrentAnimationDirection(OldVelocity, ViewInfo);

	if (OldVelocity.Size() > 0.0f || GetMovementComponent()->IsFalling())
	{
		switch (CurrentAnimationDirection)
		{
		case HopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(Flipbooks.WalkUp);
			break;
		case HopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(Flipbooks.WalkDown);
			break;
		case HopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(Flipbooks.WalkLeft);
			break;
		case HopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(Flipbooks.WalkRight);
			break;
		case HopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(Flipbooks.WalkUpLeft);
			break;
		case HopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(Flipbooks.WalkUpRight);
			break;
		case HopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(Flipbooks.WalkDownLeft);
			break;
		case HopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(Flipbooks.WalkDownRight);
			break;
		default:
			break;
		}
	}
	else
	{
		switch (CurrentAnimationDirection)
		{
		case HopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(Flipbooks.IdleUp);
			break;
		case HopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(Flipbooks.IdleDown);
			break;
		case HopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(Flipbooks.IdleLeft);
			break;
		case HopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(Flipbooks.IdleRight);
			break;
		case HopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(Flipbooks.IdleUpLeft);
			break;
		case HopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(Flipbooks.IdleUpRight);
			break;
		case HopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(Flipbooks.IdleDownLeft);
			break;
		case HopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(Flipbooks.IdleDownRight);
			break;
		default:
			break;
		}
	}

	if (GetCharacterMovement()->IsFalling())
	{
		GetSprite()->SetPlayRate(0.f);
		GetSprite()->SetPlaybackPositionInFrames(0, true);
	}
	else
	{
		GetSprite()->SetPlayRate(1.f);
	}
}

void AHopperBaseCharacter::SetCurrentAnimationDirection(const FVector& Velocity, TOptional<FMinimalViewInfo> ViewInfo)
{
	if (!ViewInfo.IsSet()) return;
	const FVector Forward = UKismetMathLibrary::GetForwardVector(ViewInfo.GetValue().Rotation);
	const FVector Right = UKismetMathLibrary::GetRightVector(ViewInfo.GetValue().Rotation);
	const float ForwardSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Forward) * 100) / 100;
	const float RightSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Right) * 100) / 100;

	bIsMoving = ForwardSpeed != 0.0f || RightSpeed != 0.0f;

	if (bIsMoving && !GetMovementComponent()->IsFalling())
	{
		if (ForwardSpeed > 0.0f && abs(RightSpeed) < 0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::Up;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::UpRight;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::UpLeft;
		}
		else if (ForwardSpeed < 0.5f && abs(RightSpeed) <= 0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::Down;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::DownRight;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::DownLeft;
		}
		else if (abs(ForwardSpeed) < 0.5f && RightSpeed > 0.0f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::Right;
		}
		else
		{
			CurrentAnimationDirection = HopperAnimationDirection::Left;
		}
	}
}
