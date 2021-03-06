// © 2021, Matthew Barham. All rights reserved.


#include "UI/HopperHUD.h"
#include "Components/ScrollBox.h"
#include "Core/Items/HopperItem.h"
#include "Components/TextBlock.h"
#include "Core/HopperPlayerController.h"
#include "UI/HopperListItem.h"

void UHopperHUD::NativeConstruct()
{
	Super::NativeConstruct();

	// set delegate to update UI here (PlayerController will be valid by this point)
	if (PlayerController)
	{
		PlayerController->OnInventoryItemChangedNative.AddUObject(
			this, &UHopperHUD::UpdateInventoryInformation);
	}
}

void UHopperHUD::UpdateInventoryInformation(bool bAdded, UHopperItem* Item)
{
	if (PlayerController)
	{
		FHopperItemData ItemData;
		if (PlayerController->GetInventoryItemData(Item, ItemData))
		{
			// Increase coin collection count
			CoinCount->SetText(FText::FromString(FString::FromInt(ItemData.ItemCount)));

			// Add to inventory list
			UHopperListItem* NewListItem = CreateWidget<UHopperListItem>(
				this, HopperListItemClass.LoadSynchronous());
			if (NewListItem)
			{
				// Check if we already have items in the UI
				if (InventoryList->HasAnyChildren())
				{
					TArray<UWidget*> ArrayOfChildren = InventoryList->GetAllChildren();
					for (UWidget* Widget : ArrayOfChildren)
					{
						UHopperListItem* ListItem = Cast<UHopperListItem>(Widget);
						if (ListItem)
						{
							if (ListItem->GetItemName().EqualToCaseIgnored(Item->ItemName))
							{
								ListItem->SetItemCount(ItemData.ItemCount);
								break;
							}
						}
					}
				}
					// Add a new item to the UI
				else
				{
					NewListItem->AddNewItemToInventoryList(Item->ItemName.ToString(), ItemData.ItemCount);
					InventoryList->AddChild(NewListItem);
				}
			}
		}
	}
}
