// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Core/HopperData.h"
#include "HopperBaseCharacter.generated.h"

class UNiagaraSystem;
class USphereComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFootstepSignature);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDeathSignature);

/**
 * Base character class
 */
UCLASS()
class HOPPER_API AHopperBaseCharacter : public APaperCharacter
{
	GENERATED_BODY()

public:
	AHopperBaseCharacter();

protected:
	/**
	 * Called upon movement, with a timer delay of 0.3 seconds.
	 * Use for footstep sound or particle effects in Blueprints.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnFootstep();
	void NotifyFootstepTaken();

	/**
	 * Called when Character dies.
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnDeath();
	void CharacterDeath() const;

	/* Broadcast when the Flipbook animation is walking */
	UPROPERTY(BlueprintAssignable, Category="Character")
	FFootstepSignature FootstepDelegate;

	/* Broadcast when Character's health reaches 0 */
	UPROPERTY(BlueprintAssignable, Category="Character")
	FCharacterDeathSignature CharacterDeathDelegate;

	/** Class Overrides */

	virtual void BeginPlay() override;
	virtual void OnJumped_Implementation() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void NotifyJumpApex() override;

	/** Movement */

	void ModifyJumpPower();
	void ResetJumpPower();

	/** Actions */

	/**
	 * Changes sprite to an Editor-set Flipbook for a punch animation, based on
	 * direction, then shifts the sprite's local transform by 25.f in the punch's
	 * direction, launches the enemy character, and spawns an emitter if provided.
	 * Resets after 0.3 seconds.
	 * @param SystemToSpawn Pointer to a Niagara System
	 * @return True if successful hit
	 */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	bool Punch(UNiagaraSystem* SystemToSpawn);

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

	/** Character Stats */
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Character Stats")
	void DamageHealth(const float Damage);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	TObjectPtr<USphereComponent> AttackSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Config")
	uint8 bIsMoving:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	HopperAnimationDirection CurrentAnimationDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperMovementFlipbooks MovementFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperPunchFlipbooks PunchFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<float> JumpPowerLevels{1200.f, 1400.f, 1800.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float AttackForce{750.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float MaxHealth{100.f};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	float Health{MaxHealth};

	FTimerHandle AttackTimer;
	FTimerHandle FootstepTimer;
	FTimerHandle JumpReset;
	float AttackRadius{150.f};
	int JumpCounter{};
	uint8 bFootstepGate:1;
	uint8 bAttackGate:1;
};
