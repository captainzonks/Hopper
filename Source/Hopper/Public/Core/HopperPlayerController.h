// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Interfaces/HopperInventoryInterface.h"
#include "HopperPlayerController.generated.h"

/**
 * Hopper Player Controller for Gameplay
 */
UCLASS()
class HOPPER_API AHopperPlayerController : public APlayerController, public IHopperInventoryInterface
{
	GENERATED_BODY()

public:
	/** Constructor and overrides */
	AHopperPlayerController()
	{
	}

	virtual void BeginPlay() override;

	
	/*******************************************
	 *               Inventory
	 ******************************************/

	/** Map of all items owned by this player, from definition to data */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Inventory")
	TMap<UHopperItem*, FHopperItemData> InventoryData;

	/** Delegate called when an inventory item has been added or removed */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryItemChanged OnInventoryItemChanged;

	/** Native version above, called before BP delegate */
	FOnInventoryItemChangedNative OnInventoryItemChangedNative;

	/** Called after the inventory was changed and we notified all delegates */
	UFUNCTION(BlueprintImplementableEvent, Category = "Inventory")
	void InventoryItemChanged(bool bAdded, UHopperItem* Item);

	/** Delegate called when the inventory has been loaded/reloaded */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryLoaded OnInventoryLoaded;

	/** Native version above, called before BP delegate */
	FOnInventoryLoadedNative OnInventoryLoadedNative;

	/** Adds a new inventory item.
	 * If the item supports count you can add more than one count.
	 * It will also update the level when adding if required */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddInventoryItem(UHopperItem* NewItem, int32 ItemCount = 1, int32 ItemLevel = 1);

	/** Remove an inventory item. A remove count of <= 0 means to remove all copies */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveInventoryItem(UHopperItem* RemovedItem, int32 RemoveCount = 1);

	/** Returns all inventory items of a given type.
	 * If none is passed as type it will return all */
	void GetInventoryItems(TArray<UHopperItem*>& Items, FPrimaryAssetType ItemType);

	/** Returns number of instances of this item found in the inventory.
	 * This uses count from GetItemData */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetInventoryItemCount(const UHopperItem* Item) const;

	/** Returns the item data associated with an item. Returns false if none found */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	bool GetInventoryItemData(const UHopperItem* Item, FHopperItemData& ItemData) const;

	/** Manually save the inventory, this is called from add/remove functions automatically */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool SaveInventory();

	/** Loads inventory from save game on game instance, this will replace arrays */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool LoadInventory();

	/*******************************************
	*    Implement IHopperInventoryInterface 
	*******************************************/
	
	virtual const TMap<UHopperItem*, FHopperItemData>& GetInventoryDataMap() const override
	{
		return InventoryData;
	}

	virtual FOnInventoryItemChangedNative& GetInventoryItemChangedDelegate() override
	{
		return OnInventoryItemChangedNative;
	}

	virtual FOnInventoryLoadedNative& GetInventoryLoadedDelegate() override
	{
		return OnInventoryLoadedNative;
	}

protected:
	/** Calls the inventory update callbacks */
	void NotifyInventoryItemChanged(bool bAdded, UHopperItem* Item);
	// void NotifySlottedItemChanged(FHopperItemData ItemSlot, UHopperItem* Item);
	void NotifyInventoryLoaded();

	/** Called when a global save game as been loaded */
	// void HandleSaveGameLoaded(UHopperSaveGame* NewSaveGame);
};
