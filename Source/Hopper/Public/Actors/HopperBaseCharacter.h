// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "Core/Hopper.h"
#include "Core/HopperData.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "PaperCharacter.h"
#include "HopperBaseCharacter.generated.h"

class UHopperGameplayAbility;
class UGameplayEffect;
class UHopperAbilitySystemComponent;
class UHopperAttributeSet;
class UNiagaraSystem;
class USphereComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFootstepSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FAttackTimerResetSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeathSignature);

/**
 * Base character class
 */
UCLASS()
class HOPPER_API AHopperBaseCharacter : public APaperCharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AHopperBaseCharacter();

	/** Actions */

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Actions")
	void HandlePunch();

	// Checks if any HopperBaseEnemy Actors are within the AttackSphere
	UFUNCTION(BlueprintPure, Category = "Actions")
	bool IsEnemyInAttackRadius() const;

	/**
	 * Plays a punch Flipbook from the character's PunchFlipbooks struct
	 * based on the CurrentAnimationDirection enum, then sets the AttackTimer
	 * to the provided float value.
	 * @param TimerValue How much time it takes before another attack can execute.
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Animation")
	void PlayPunchAnimation(const float TimerValue = 0.3f);

	/** Returns current health, will be 0 if dead */
	UFUNCTION(BlueprintCallable)
	virtual float GetHealth() const;

	/** Returns maximum health, health will never be greater than this */
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const;

protected:
	/** Class Overrides */

	virtual void BeginPlay() override;
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/******************
	 * Ability System
	 ******************/

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	virtual void AddStartupGameplayAbilities();

	/** Passive gameplay effects applied on creation */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UGameplayEffect>> PassiveGameplayEffects;

	/** Abilities to grant to this character on creation. These will be activated by tag or event and are not bound to specific inputs */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Abilities)
	TArray<TSubclassOf<UHopperGameplayAbility>> GameplayAbilities;

	/** If true we have initialized our abilities */
	UPROPERTY()
	uint8 bAbilitiesInitialized:1;
	
	/**
	 * Called when character takes damage, which may have killed them
	 *
	 * @param DamageAmount Amount of damage that was done, not clamped based on current health
	 * @param HitInfo The hit info that generated this damage
	 * @param DamageTags The gameplay tags of the event that did the damage
	 * @param InstigatorCharacter The character that initiated this damage
	 * @param DamageCauser The actual actor that did the damage, might be a weapon or projectile
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDamaged(float DamageAmount, const FHitResult& HitInfo, const struct FGameplayTagContainer& DamageTags,
	               AHopperBaseCharacter* InstigatorCharacter, AActor* DamageCauser);

	/**
	 * Called when health is changed, either from healing or from being damaged
	 * For damage this is called in addition to OnDamaged/OnKilled
	 *
	 * @param DeltaValue Change in health value, positive for heal, negative for cost. If 0 the delta is unknown
	 * @param EventTags The gameplay tags of the event that changed mana
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	// Called from HopperAttributeSet, these call BP events above
	virtual void HandleDamage(float DamageAmount, const FHitResult& HitInfo,
	                          const struct FGameplayTagContainer& DamageTags, AHopperBaseCharacter* InstigatorCharacter,
	                          AActor* DamageCauser);
	virtual void HandleHealthChanged(float DeltaValue, const struct FGameplayTagContainer& EventTags);

	/**
	 * Called upon movement, with a timer delay of 0.3 seconds.
	 * Use for footstep sound or particle effects in Blueprints.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnFootstep();
	void NotifyFootstepTaken();

	/**
	 * Called when Character dies.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnDeath();
	void CharacterDeath() const;

	UFUNCTION(BlueprintImplementableEvent)
	void OnAttackTimerEnd();
	void AttackTimerReset() const;

	/* Broadcast when the Flipbook animation is walking */
	UPROPERTY(BlueprintAssignable, Category="Character")
	FFootstepSignature FootstepDelegate;

	/* Broadcast when Character's health reaches 0 */
	UPROPERTY(BlueprintAssignable, Category="Character")
	FCharacterDeathSignature CharacterDeathDelegate;

	UPROPERTY(BlueprintAssignable, Category = "Character")
	FAttackTimerResetSignature AttackTimerDelegate;

	/** Movement */

	void ModifyJumpPower();
	void ResetJumpPower();

	/** Animation */

	/**
	 * Animates the sprite with Editor-set Flipbooks for movement. This function is called
	 * by binding it to the OnCharacterMovementUpdated delegate. Direction is selected by
	 * calling SetCurrentAnimationDirection().
	 * @param DeltaTime Time since last frame.
	 * @param OldLocation Location at call.
	 * @param OldVelocity Velocity at call.
	 */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void Animate(float DeltaTime, FVector OldLocation, const FVector OldVelocity);

	/**
	 * Sets the CurrentAnimationDirection enum by detecting velocity and the Player's camera rotation
	 * in world space via the provided ViewInfo. If ViewInfo is not set, the function returns early.
	 * @param Velocity Actor's velocity at time of call.
	 * @param ViewInfo Player's camera information.
	 */
	virtual void SetCurrentAnimationDirection(const FVector& Velocity, TOptional<FMinimalViewInfo> ViewInfo);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<USphereComponent> AttackSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component")
	TObjectPtr<UHopperAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UHopperAttributeSet> Attributes;

	// Friended to allow access to handle functions above
	friend UHopperAttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bIsMoving:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	EHopperAnimationDirection CurrentAnimationDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperMovementFlipbooks MovementFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperPunchFlipbooks PunchFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<float> JumpPowerLevels{1200.f, 1400.f, 1800.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float AttackForce{750.f};

	FTimerHandle AttackTimer;
	FTimerHandle FootstepTimer;
	FTimerHandle JumpReset;
	float AttackRadius{150.f};
	int JumpCounter{};
	uint8 bFootstepGate:1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bAttackGate:1;
};
