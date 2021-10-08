// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PaperCharacter.h"
#include "Core/HopperData.h"
#include "HopperBaseCharacter.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFootstepSignature);

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

	/* Broadcast when the Flipbook animation is walking */
	UPROPERTY(BlueprintAssignable, Category="Character")
	FFootstepSignature FootstepDelegate;

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
	 * direction. Resets after 0.3 seconds.
	 */
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Punch();

	/** Animation */

	/**
	 * Animates the sprite with Editor-selected Flipbooks for movement. This function is called
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
	uint8 bIsMoving:1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	HopperAnimationDirection CurrentAnimationDirection;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperMovementFlipbooks MovementFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FHopperPunchFlipbooks PunchFlipbooks;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TArray<float> JumpPowerLevels{1200.f, 1400.f, 1800.f};

	FTimerHandle AttackTimer;
	FTimerHandle FootstepTimer;
	FTimerHandle JumpReset;
	int JumpCounter{};
	uint8 bFootstepGate:1;
	uint8 bAttackGate:1;
};
