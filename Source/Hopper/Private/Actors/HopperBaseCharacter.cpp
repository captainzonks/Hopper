// © 2021, Matthew Barham. All rights reserved.

#include "Actors/HopperBaseCharacter.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig.h"

AHopperBaseCharacter::AHopperBaseCharacter()
{
	bReplicates = true;

	bAbilitiesInitialized = false;
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

	AbilitySystemComponent = CreateDefaultSubobject<UHopperAbilitySystemComponent>(TEXT("Ability System Component"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	Attributes = CreateDefaultSubobject<UHopperAttributeSet>(TEXT("Attributes"));

	DeadTag = FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead");


}

void AHopperBaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	SetReplicateMovement(true);
	OnFootstepTakenNative.AddUObject(this, &AHopperBaseCharacter::OnFootstepNative);
	OnAttackTimerEndNative.AddUObject(this, &AHopperBaseCharacter::OnAttackEndNative);
	OnCharacterDeathNative.AddUObject(this, &AHopperBaseCharacter::OnDeathNative);
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

void AHopperBaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Server GAS init
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AddStartupGameplayAbilities();
	}
}

void AHopperBaseCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	// called in SetupPlayerInputComponent() for redundancy
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			"EHopperAbilityInputID",
			static_cast<int32>(EHopperAbilityInputID::Confirm),
			static_cast<int32>(EHopperAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AHopperBaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// called in OnRep_PlayerState() for redundancy
	if (AbilitySystemComponent && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			"EHopperAbilityInputID",
			static_cast<int32>(EHopperAbilityInputID::Confirm),
			static_cast<int32>(EHopperAbilityInputID::Cancel));

		AbilitySystemComponent->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

void AHopperBaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AHopperBaseCharacter::ModifyJumpPower()
{
	switch (JumpCounter)
	{
	case 1:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[1];
		UE_LOG(LogHopper, Display, TEXT("Jump Power Level 1"))
		break;
	case 2:
		GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[2];
		UE_LOG(LogHopper, Display, TEXT("Jump Power Level 2"))
		break;
	default:
		UE_LOG(LogHopper, Display, TEXT("Jump Power Level 0"))
		break;
	}
}

void AHopperBaseCharacter::ResetJumpPower()
{
	JumpCounter = 0;
	GetCharacterMovement()->JumpZVelocity = JumpPowerLevels[0];
	UE_LOG(LogHopper, Display, TEXT("Jump Power Reset"))
}

void AHopperBaseCharacter::HandlePunch_Implementation()
{
	TArray<AActor*> ActorsArray;
	AttackSphere->GetOverlappingActors(ActorsArray);
	int Count{};

	if (ActorsArray.Num() > 0)
	{
		for (AActor* Actor : ActorsArray)
		{
			if (Actor &&
				Actor != this &&
				Actor->ActorHasTag("Enemy") &&
				UKismetSystemLibrary::DoesImplementInterface(
					Actor, UHopperCharacterInterface::StaticClass()) &&
				UKismetSystemLibrary::DoesImplementInterface(
					Actor, UAbilitySystemInterface::StaticClass()))
			{
				// don't punch if dead
				if (Cast<IAbilitySystemInterface>(Actor)->GetAbilitySystemComponent()->HasMatchingGameplayTag(
					FGameplayTag::RequestGameplayTag("Gameplay.Status.IsDead")))
				{
					UE_LOG(LogHopper, Log, TEXT("Found IsDead"))
					continue;
				}
				
				UE_LOG(LogHopper, Log, TEXT("Applying Punch Force"))
				Cast<IHopperCharacterInterface>(Actor)->ApplyPunchForceToCharacter(GetActorLocation(), AttackForce);

				const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.Hit");
				FGameplayEventData Payload = FGameplayEventData();
				Payload.Instigator = GetInstigator();
				Payload.Target = Actor;
				Payload.TargetData = UAbilitySystemBlueprintLibrary::AbilityTargetDataFromActor(Actor);
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);
				
				++Count;
			}
		}
	}

	// if our Count returns 0, it means we did not hit an enemy and we should end our ability
	if (Count == 0)
	{
		const FGameplayTag Tag = FGameplayTag::RequestGameplayTag("Weapon.NoHit");
		FGameplayEventData Payload = FGameplayEventData();
		Payload.Instigator = GetInstigator();
		Payload.TargetData = FGameplayAbilityTargetDataHandle();
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetInstigator(), Tag, Payload);
	}
}

void AHopperBaseCharacter::ApplyPunchForceToCharacter(const FVector FromLocation, const float InAttackForce) const
{
	const FVector TargetLocation = GetActorLocation();
	const FVector Direction = UKismetMathLibrary::GetDirectionUnitVector(FromLocation, TargetLocation);

	GetCharacterMovement()->Launch(FVector(
		Direction.X * AttackForce,
		Direction.Y * AttackForce,
		abs(Direction.Z + 1) * AttackForce));
}

void AHopperBaseCharacter::OnFootstepNative()
{
	if (bFootstepGate)
	{
		bFootstepGate = !bFootstepGate;
		if (OnFootstepTaken.IsBound())
		{
			OnFootstepTaken.Broadcast();
		}
		GetWorldTimerManager().SetTimer(FootstepTimer, [this]() { bFootstepGate = true; }, 0.3f, false);
	}
}

void AHopperBaseCharacter::OnDeathNative()
{
	if (OnCharacterDeath.IsBound())
	{
		OnCharacterDeath.Broadcast();
	}
}

void AHopperBaseCharacter::OnAttackEndNative()
{
	if (OnAttackTimerEnd.IsBound())
	{
		OnAttackTimerEnd.Broadcast();
	}
}

float AHopperBaseCharacter::GetHealth() const
{
	if (!Attributes)
		return 1.f;

	return Attributes->GetHealth();
}

float AHopperBaseCharacter::GetMaxHealth() const
{
	return Attributes->GetMaxHealth();
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
		case EHopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUp);
			break;
		case EHopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDown);
			break;
		case EHopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkLeft);
			break;
		case EHopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkRight);
			break;
		case EHopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpLeft);
			break;
		case EHopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkUpRight);
			break;
		case EHopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownLeft);
			break;
		case EHopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.WalkDownRight);
			break;
		default:
			break;
		}

		if (!GetCharacterMovement()->IsFalling())
		{
			if (OnFootstepTakenNative.IsBound())
			{
				OnFootstepTakenNative.Broadcast();
			}
		}
	}
	else
	{
		switch (CurrentAnimationDirection)
		{
		case EHopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUp);
			break;
		case EHopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDown);
			break;
		case EHopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleLeft);
			break;
		case EHopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleRight);
			break;
		case EHopperAnimationDirection::UpLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpLeft);
			break;
		case EHopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleUpRight);
			break;
		case EHopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(MovementFlipbooks.IdleDownLeft);
			break;
		case EHopperAnimationDirection::DownRight:
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
	FVector Forward;
	FVector Right;
	if (ViewInfo.IsSet())
	{
		Forward = UKismetMathLibrary::GetForwardVector(ViewInfo.GetValue().Rotation);
		Right = UKismetMathLibrary::GetRightVector(ViewInfo.GetValue().Rotation);
	}
	else
	{
		Forward = GetActorForwardVector().GetSafeNormal();
		Right = GetActorRightVector().GetSafeNormal();
	}

	const float ForwardSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Forward) * 100) / 100;
	const float RightSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Right) * 100) / 100;

	bIsMoving = ForwardSpeed != 0.0f || RightSpeed != 0.0f;

	if (bIsMoving && !GetCharacterMovement()->IsFalling())
	{
		if (ForwardSpeed > 0.0f && abs(RightSpeed) < 0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::Up;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::UpRight;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::UpLeft;
		}
		else if (ForwardSpeed < 0.5f && abs(RightSpeed) <= 0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::Down;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed >= 0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::DownRight;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed <= -0.5f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::DownLeft;
		}
		else if (abs(ForwardSpeed) < 0.5f && RightSpeed > 0.0f)
		{
			CurrentAnimationDirection = EHopperAnimationDirection::Right;
		}
		else
		{
			CurrentAnimationDirection = EHopperAnimationDirection::Left;
		}
	}
}

void AHopperBaseCharacter::PlayPunchAnimation_Implementation(const float TimerValue)
{
	FVector NewLocation{GetSprite()->GetRelativeLocation()};

	if (bAttackGate)
	{
		switch (CurrentAnimationDirection)
		{
		case EHopperAnimationDirection::Down:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDown);
			NewLocation.X -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::Up:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUp);
			NewLocation.X += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::Right:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchRight);
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::Left:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchLeft);
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::DownRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownRight);
			NewLocation.X -= 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::DownLeft:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchDownLeft);
			NewLocation.X -= 25.f;
			NewLocation.Y -= 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::UpRight:
			GetSprite()->SetFlipbook(PunchFlipbooks.PunchUpRight);
			NewLocation.X += 25.f;
			NewLocation.Y += 25.f;
			GetSprite()->SetRelativeLocation(NewLocation);
			break;
		case EHopperAnimationDirection::UpLeft:
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
			                                if (OnAttackTimerEndNative.IsBound())
			                                {
				                                OnAttackTimerEndNative.Broadcast();
			                                }
		                                },
		                                TimerValue, false);
	}
}

UAbilitySystemComponent* AHopperBaseCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AHopperBaseCharacter::AddStartupGameplayAbilities()
{
	check(AbilitySystemComponent);

	if (GetLocalRole() == ROLE_Authority && !bAbilitiesInitialized)
	{
		// Grant abilities, but only on the server	
		for (TSubclassOf<UHopperGameplayAbility>& StartupAbility : GameplayAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(
				StartupAbility, 1, static_cast<int32>(StartupAbility.GetDefaultObject()->AbilityInputID), this));
		}

		// Now apply passives
		for (const TSubclassOf<UGameplayEffect>& GameplayEffect : PassiveGameplayEffects)
		{
			FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle NewHandle = AbilitySystemComponent->MakeOutgoingSpec(
				GameplayEffect, 1, EffectContext);
			if (NewHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveGEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(
					*NewHandle.Data.Get(), AbilitySystemComponent);
			}
		}

		bAbilitiesInitialized = true;
	}
}

void AHopperBaseCharacter::HandleDamage(float DamageAmount, const FHitResult& HitInfo,
                                        const FGameplayTagContainer& DamageTags,
                                        AHopperBaseCharacter* InstigatorCharacter, AActor* DamageCauser)
{
	OnDamaged(DamageAmount, HitInfo, DamageTags, InstigatorCharacter, DamageCauser);
}

void AHopperBaseCharacter::HandleHealthChanged(float DeltaValue, const FGameplayTagContainer& EventTags)
{
	// We only call the BP callback if this is not the initial ability setup
	if (bAbilitiesInitialized)
	{
		OnHealthChanged(DeltaValue, EventTags);
		if (GetHealth() <= 0)
		{
			UE_LOG(LogHopper, Warning, TEXT("Adding DeadTag"))
			AbilitySystemComponent->AddLooseGameplayTag(DeadTag);
		}
	}
}
