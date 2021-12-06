// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "Core/Hopper.h"
#include "HopperInventoryInterface.generated.h"

/**
 * Interface for actors that provide a set of HopperItems bound to ItemSlots
 * This exists so HopperCharacterBase can query inventory without doing hacky player controller casts
 * It is designed only for use by native classes
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceBlueprint))
class UHopperInventoryInterface : public UInterface
{
	GENERATED_BODY()
};

class HOPPER_API IHopperInventoryInterface
{
	GENERATED_BODY()

public:
	/** Returns the map of items to data */
	virtual const TMap<UHopperItem*, FHopperItemData>& GetInventoryDataMap() const = 0;

	/** Returns the map of slots to items */
	// virtual const TMap<FHopperItemSlot, UHopperItem*>& GetSlottedItemMap() const = 0;

	/** Gets the delegate for inventory item changes */
	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() = 0;

	/** Gets the delegate for inventory slot changes */
	// virtual FOnSlottedItemChangedNative& GetSlottedItemChangedDelegate() = 0;

	/** Gets the delegate for when the inventory loads */
	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() = 0;
};
