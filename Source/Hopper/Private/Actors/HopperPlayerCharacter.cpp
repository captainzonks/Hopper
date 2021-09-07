// © 2021, Matthew Barham. All rights reserved.


#include "Actors/HopperPlayerCharacter.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

AHopperPlayerCharacter::AHopperPlayerCharacter()
{
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	bUseControllerRotationYaw = false;

	CameraSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm"));
	CameraSpringArm->SetupAttachment(GetCapsuleComponent());
	CameraSpringArm->TargetArmLength = 1500.f;
	CameraSpringArm->SetRelativeRotation(FRotator(-20.f, 0.f, 0.f));
	CameraSpringArm->TargetOffset = FVector(0.f, 0.f, 200.f);
	CameraSpringArm->bDoCollisionTest = false;
	CameraSpringArm->bUsePawnControlRotation = true;
	CameraSpringArm->bInheritPitch = true;
	CameraSpringArm->bInheritRoll= false;
	CameraSpringArm->bInheritYaw = true;
	CameraSpringArm->bEnableCameraLag = true;
	CameraSpringArm->bEnableCameraRotationLag = true;
	CameraSpringArm->CameraLagSpeed = 4.f;
	CameraSpringArm->CameraRotationLagSpeed = 8.f;
	CameraSpringArm->CameraLagMaxDistance = 600.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(CameraSpringArm);
	Camera->FieldOfView = 55.f;
	Camera->PostProcessBlendWeight = 1.f;
	Camera->PostProcessSettings.bOverride_DepthOfFieldFstop = true;
	Camera->PostProcessSettings.DepthOfFieldFstop = 0.5f;
	Camera->PostProcessSettings.bOverride_DepthOfFieldSensorWidth = true;
	Camera->PostProcessSettings.DepthOfFieldSensorWidth = 100.f;
	Camera->PostProcessSettings.bOverride_DepthOfFieldFocalDistance = true;
	Camera->PostProcessSettings.DepthOfFieldFocalDistance = CameraSpringArm->TargetArmLength;
	Camera->PostProcessSettings.bOverride_DepthOfFieldMinFstop = true;
	Camera->PostProcessSettings.DepthOfFieldMinFstop = 0.f;
	Camera->PostProcessSettings.MotionBlurAmount = 0.f;
}

void AHopperPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AHopperPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	/* Movement Controls */
	PlayerInputComponent->BindAxis(TEXT("Up"), this, &AHopperPlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis(TEXT("Right"), this, &AHopperPlayerCharacter::MoveRight);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &ACharacter::Jump);

	/* Camera Controls */
	PlayerInputComponent->BindAxis(TEXT("LookUp"), this, &AHopperPlayerCharacter::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Turn"), this, &AHopperPlayerCharacter::Turn);
}

void AHopperPlayerCharacter::SetCurrentAnimationDirection(const FVector& Velocity, TOptional<FMinimalViewInfo> ViewInfo)
{
	const FVector Forward = GetActorForwardVector().GetSafeNormal();
	const FVector Right = GetActorRightVector().GetSafeNormal();
	const float ForwardSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Forward) * 100) / 100;
	const float RightSpeed = FMath::Floor(FVector::DotProduct(Velocity.GetSafeNormal(), Right) * 100) / 100;

	bIsMoving = ForwardSpeed != 0.0f || RightSpeed != 0.0f;

	if (bIsMoving)
	{
		if (ForwardSpeed > 0.0f && abs(RightSpeed) < 0.25f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::Up;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed >= 0.25f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::UpRight;
		}
		else if (ForwardSpeed > 0.5f && RightSpeed <= -0.25f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::UpLeft;
		}
		else if (ForwardSpeed < 0.5f && abs(RightSpeed) <= 0.25f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::Down;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed >= 0.25f)
		{
			CurrentAnimationDirection = HopperAnimationDirection::DownRight;
		}
		else if (ForwardSpeed < -0.5f && RightSpeed <= -0.25f)
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

void AHopperPlayerCharacter::MoveForward(float Value)
{
	if (Value != 0.f)
	{
		const FVector Direction = Camera->GetForwardVector();
		AddMovementInput(Direction, Value);
	}
}

void AHopperPlayerCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		const FVector Direction = Camera->GetRightVector();
		AddMovementInput(Direction, Value);
	}
}

void AHopperPlayerCharacter::LookUp(float Value)
{
	if (Value != 0.f)
	{
		AddControllerPitchInput(Value);
	}
}

void AHopperPlayerCharacter::Turn(float Value)
{
	if (Value != 0.f)
	{
		AddControllerYawInput(Value);
	}
}
