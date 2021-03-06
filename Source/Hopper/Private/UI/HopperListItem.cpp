// © 2021, Matthew Barham. All rights reserved.


#include "UI/HopperListItem.h"
#include "Components/TextBlock.h"

void UHopperListItem::AddNewItemToInventoryList(const FString InItemName, const int InItemCount) const
{
	if (ItemName && ItemCount)
	{
		ItemName->SetText(FText::FromString(InItemName));
		ItemCount->SetText(FText::FromString(FString::FromInt(InItemCount)));
	}
}

void UHopperListItem::SetItemCount(const int NewCount) const
{
	if (ItemCount)
	{
		ItemCount->SetText(FText::FromString(FString::FromInt(NewCount)));
	}
}
