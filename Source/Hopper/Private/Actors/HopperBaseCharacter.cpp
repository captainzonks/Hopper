// © 2021, Matthew Barham. All rights reserved.

#include "Actors/HopperBaseCharacter.h"

#include "PaperFlipbookComponent.h"
#include "Actors/HopperEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "Core/HopperGameMode.h"

AHopperBaseCharacter::AHopperBaseCharacter()
{
	bReplicates = true;

	bFootstepGate = true;
	bAttackGate = true;

	OnCharacterMovementUpdated.AddDynamic(this, &AHopperBaseCharacter::Animate);

	AttackSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Attack Sphere"));
	AttackSphere->SetupAttachment(RootComponent);
	AttackSphere->SetSphereRadius(AttackRadius);

	GetCharacterMovement()->GravityScale = 2.8f;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];

	GetCapsuleComponent()->SetCapsuleRadius(70.f);

	GetSprite()->SetRelativeScale3D(FVector(11.f, 11.f, 11.f));
	GetSprite()->SetUsingAbsoluteRotation(true);
	GetSprite()->SetFlipbook(MovementFlipbooks.IdleDown);
	GetSprite()->CastShadow = true;
}

void AHopperBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttackInterface = Cast<IHopperAttackInterface>(UGameplayStatics::GetGameMode(this));

	SetReplicateMovement(true);
	FootstepDelegate.AddDynamic(this, &AHopperBaseCharacter::OnFootstep);
	CharacterDeathDelegate.AddDynamic(this, &AHopperBaseCharacter::OnDeath);
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

void AHopperBaseCharacter::NotifyFootstepTaken()
{
	if (bFootstepGate)
	{
		bFootstepGate = !bFootstepGate;
		if (FootstepDelegate.IsBound())
			FootstepDelegate.Broadcast();
		GetWorldTimerManager().SetTimer(FootstepTimer, [this]() { bFootstepGate = true; }, 0.3f, false);
	}
}

void AHopperBaseCharacter::CharacterDeath() const
{
	if (CharacterDeathDelegate.IsBound())
		CharacterDeathDelegate.Broadcast();
}

bool AHopperBaseCharacter::Punch(UNiagaraSystem* SystemToSpawn)
{
	bool bHit{false};
	FVector NewLocation{GetSprite()->GetRelativeLocation()};

	if (bAttackGate)
	{
		switch (CurrentAnimationDirection)
		{
		case HopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDown);
			NewLocation.X -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUp);
			NewLocation.X += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchRight);
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchLeft);
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownRight);
			NewLocation.X -= 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownLeft);
			NewLocation.X -= 25.f;
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUpRight);
			NewLocation.X += 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case HopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUpLeft);
			NewLocation.X += 25.f;
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		default:
			break;
		}

		bAttackGate = false;
		GetWorldTimerManager().SetTimer(AttackTimer,
		                                [this]()
		                                {
			                                bAttackGate = true;
			                                GetSprite()->SetRelativeLocation(FVector::ZeroVector);
		                                },
		                                0.3f, false);

		TArray<AActor*> OverlappingActors;
		AttackSphere->GetOverlappingActors(OverlappingActors, AHopperEnemy::StaticClass());
		if (OverlappingActors.Num() > 0)
		{
			bHit = true;
			for (AActor* Actor : OverlappingActors)
			{
				AHopperEnemy* HopperEnemy = Cast<AHopperEnemy>(Actor);
				const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(
					GetActorLocation(), HopperEnemy->GetActorLocation());
				HopperEnemy->LaunchCharacter(FVector(
					                             Direction.X * AttackForce,
					                             Direction.Y * AttackForce,
					                             (Direction.Z + 1.f) * AttackForce),
				                             false, false);
				if (IsValid(SystemToSpawn))
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						this,
						SystemToSpawn,
						HopperEnemy->GetActorLocation());
				}

				FHopperAttack Attack;
				Attack.Target = HopperEnemy;
				Attack.AttackType = HopperAttackType::Melee;
				AttackInterface->Attack(Attack);
				// HopperEnemy->DamageHealth(HopperEnemy->GetMaxHealth() / 3);
			}
		}
	}

	return bHit;
}

void AHopperBaseCharacter::Animate(float DeltaTime, FVector OldLocation, const FVector OldVelocity)
{
	if (!bAttackGate) return;

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

	if (OldVelocity.Size() > 0.0f || GetCharacterMovement()->IsFalling())
	{
		switch (CurrentAnimationDirection)
		{
		case HopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUp);
			break;
		case HopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDown);
			break;
		case HopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkLeft);
			break;
		case HopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkRight);
			break;
		case HopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpLeft);
			break;
		case HopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpRight);
			break;
		case HopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownLeft);
			break;
		case HopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownRight);
			break;
		default:
			break;
		}

		if (!GetCharacterMovement()->IsFalling())
		{
			NotifyFootstepTaken();
		}
	}
	else
	{
		switch (CurrentAnimationDirection)
		{
		case HopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUp);
			break;
		case HopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDown);
			break;
		case HopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleLeft);
			break;
		case HopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleRight);
			break;
		case HopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpLeft);
			break;
		case HopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpRight);
			break;
		case HopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDownLeft);
			break;
		case HopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDownRight);
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

	if (bIsMoving && !GetCharacterMovement()->IsFalling())
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

void AHopperBaseCharacter::DamageHealth(const float Damage)
{
	Health -= Damage;
	UE_LOG(LogTemp, Warning, TEXT("Health: %f"), Health)
	if (Health <= 0)
	{
		CharacterDeath();
	}
}
