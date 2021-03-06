// © 2021, Matthew Barham. All rights reserved.


#include "Core/HopperPlayerController.h"
#include "Core/Items/HopperItem.h"

bool AHopperPlayerController::AddInventoryItem(UHopperItem* NewItem, const int32 ItemCount, const int32 ItemLevel)
{
	bool bChanged{false};
	if (!NewItem)
	{
		UE_LOG(LogHopper, Warning, TEXT("AddInventoryItem: Failed trying to add null item!"))
		return false;
	}

	if (ItemCount <= 0 || ItemLevel <= 0)
	{
		UE_LOG(LogHopper, Warning, TEXT("AddInventoryItem: Failed trying to add item %s with negative count or level!"),
		       *NewItem->GetName())
		return false;
	}

	// Find current item data, which may be empty
	FHopperItemData OldData;
	GetInventoryItemData(NewItem, OldData);

	// Find modified data
	FHopperItemData NewData{OldData};
	NewData.UpdateItemData(FHopperItemData(ItemCount, ItemLevel), NewItem->MaxCount, NewItem->MaxLevel);

	if (OldData != NewData)
	{
		//If data changed, need to update storage and call callback
		InventoryData.Add(NewItem, NewData);
		NotifyInventoryItemChanged(true, NewItem);
		bChanged = true;
	}

	// if (bAutoSlot)

	if (bChanged)
	{
		// If anything changed, write to save game
		SaveInventory();
		return true;
	}

	return false;
}

bool AHopperPlayerController::RemoveInventoryItem(UHopperItem* RemovedItem, const int32 RemoveCount)
{
	if (!RemovedItem)
	{
		UE_LOG(LogHopper, Warning, TEXT("RemoveInventoryItem: Failed trying to remove null item!"))
		return false;
	}

	// Find current item data, which may be empty
	FHopperItemData NewData;
	GetInventoryItemData(RemovedItem, NewData);

	if (!NewData.IsValid())
	{
		// Wasn't found
		return false;
	}

	// If RemoveCount <= 0, delete all
	if (RemoveCount <= 0)
	{
		NewData.ItemCount = 0;
	}
	else
	{
		NewData.ItemCount -= RemoveCount;
	}

	if (NewData.ItemCount > 0)
	{
		// Update data with new count
		InventoryData.Add(RemovedItem, NewData);
	}
	else
	{
		// Remove item entirely, make sure it is unslotted
		InventoryData.Remove(RemovedItem);

		// for (TPair<FHopperItemSlot, UHopperItem*>& Pair : SlottedItems)
		// ...
	}

	// If we got this far, there is a change so notify and save
	NotifyInventoryItemChanged(false, RemovedItem);

	SaveInventory();
	return true;
}

void AHopperPlayerController::GetInventoryItems(TArray<UHopperItem*>& Items, const FPrimaryAssetType ItemType)
{
	for (const TPair<UHopperItem*, FHopperItemData>& Pair : InventoryData)
	{
		if (Pair.Key)
		{
			FPrimaryAssetId AssetId{Pair.Key->GetPrimaryAssetId()};

			// Filters based on item type
			if (AssetId.PrimaryAssetType == ItemType || !ItemType.IsValid())
			{
				Items.Add(Pair.Key);
			}
		}
	}
}

int32 AHopperPlayerController::GetInventoryItemCount(const UHopperItem* Item) const
{
	const FHopperItemData* FoundItem{InventoryData.Find(Item)};

	if (FoundItem)
	{
		return FoundItem->ItemCount;
	}

	return 0;
}

bool AHopperPlayerController::GetInventoryItemData(const UHopperItem* Item, FHopperItemData& ItemData) const
{
	const FHopperItemData* FoundItem{InventoryData.Find(Item)};

	if (FoundItem)
	{
		ItemData = *FoundItem;
		return true;
	}

	ItemData = FHopperItemData(0, 0);
	return false;
}

bool AHopperPlayerController::SaveInventory()
{
	// ...

	return false;
}

bool AHopperPlayerController::LoadInventory()
{
	// ...

	return false;
}

void AHopperPlayerController::NotifyInventoryItemChanged(const bool bAdded, UHopperItem* Item)
{
	// Notify native before blueprint
	OnInventoryItemChangedNative.Broadcast(bAdded, Item);
	OnInventoryItemChanged.Broadcast(bAdded, Item);

	// Call BP update event
	InventoryItemChanged(bAdded, Item);
}

void AHopperPlayerController::NotifyInventoryLoaded()
{
	// Notify native before blueprint
	OnInventoryLoadedNative.Broadcast();
	OnInventoryLoaded.Broadcast();
}

// void AHopperPlayerController::HandleSaveGameLoaded(UHopperSaveGame* NewSaveGame)
// {
// 	LoadInventory();
// }

void AHopperPlayerController::BeginPlay()
{
	// Load inventory off save game before starting play
	LoadInventory();

	Super::BeginPlay();
}
