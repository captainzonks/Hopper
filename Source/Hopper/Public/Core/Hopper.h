// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

// Engine Includes
#include "CoreMinimal.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "PaperFlipbookComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PawnMovementComponent.h"

// Game Includes
#include "Core/HopperData.h"
#include "Core/Interfaces/HopperCharacterInterface.h"
#include "Core/Interfaces/HopperInventoryInterface.h"
#include "Core/Abilities/HopperAttributeSet.h"
#include "Core/HopperGameMode.h"
#include "Core/Abilities/HopperGameplayAbility.h"
#include "Core/Components/HopperAbilitySystemComponent.h"

HOPPER_API DECLARE_LOG_CATEGORY_EXTERN(LogHopper, Log, All);
