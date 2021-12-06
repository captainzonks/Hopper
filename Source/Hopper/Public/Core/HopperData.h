// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "HopperData.generated.h"

class AHopperBaseCharacter;
class UPaperFlipbook;
class UHopperItem;

UENUM(BlueprintType)
enum class EHopperAnimationDirection : uint8
{
	Down,
	Up,
	Right,
	Left,
	DownRight,
	DownLeft,
	UpRight,
	UpLeft
};

UENUM(BlueprintType)
enum class EHopperAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Punch
};

USTRUCT(BlueprintType)
struct HOPPER_API FHopperMovementFlipbooks
{
	GENERATED_BODY()

	// Idle

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleDownRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleDownLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleUpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> IdleUpLeft;

	// Walking

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkDownRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkDownLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkUpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> WalkUpLeft;
};

USTRUCT(BlueprintType)
struct HOPPER_API FHopperPunchFlipbooks
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchDown;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchUp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchDownRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchDownLeft;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchUpRight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UPaperFlipbook> PunchUpLeft;
};

USTRUCT(BlueprintType)
struct HOPPER_API FHopperItemData
{
	GENERATED_BODY()

	/** Constructor, default to count/level 1 so declaring them in blueprints gives you the expected behavior */
	FHopperItemData()
		: ItemCount(1),
		  ItemLevel(1)
	{
	}

	FHopperItemData(const int32 InItemCount, const int32 InItemLevel)
		: ItemCount(InItemCount),
		  ItemLevel(InItemLevel)
	{
	}

	/** The number of instances of this item in the inventory, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemCount;

	/** The level of this item. This level is shared for all instances, can never be below 1 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Item)
	int32 ItemLevel;

	/** Equality operators */
	bool operator==(const FHopperItemData& Other) const
	{
		return ItemCount == Other.ItemCount && ItemLevel == Other.ItemLevel;
	}
	bool operator!=(const FHopperItemData& Other) const
	{
		return !(*this == Other);
	}

	/** Returns true if count is greater than 0 */
	bool IsValid() const
	{
		return ItemCount > 0;
	}

	/** Append an item data, this adds the count and overrides everything else */
	void UpdateItemData(const FHopperItemData& Other, int32 MaxCount, int32 MaxLevel)
	{
		if (MaxCount <= 0)
		{
			MaxCount = MAX_int32;
		}

		if (MaxLevel <= 0)
		{
			MaxLevel = MAX_int32;
		}

		ItemCount = FMath::Clamp(ItemCount + Other.ItemCount, 1, MaxCount);
		ItemLevel = FMath::Clamp(Other.ItemLevel, 1, MaxLevel);
	}
};

/** Delegate called when an inventory item changes */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChanged, bool, bAdded, UHopperItem*, Item);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnInventoryItemChangedNative, bool, UHopperItem*);

/** Delegate called when the entire inventory has been loaded, all items may have been replaced */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryLoaded);
DECLARE_MULTICAST_DELEGATE(FOnInventoryLoadedNative);
