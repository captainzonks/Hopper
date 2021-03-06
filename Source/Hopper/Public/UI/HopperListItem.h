// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "HopperListItem.generated.h"

class UHopperItem;
class UTextBlock;
/**
 * 
 */
UCLASS()
class HOPPER_API UHopperListItem : public UUserWidget
{
	GENERATED_BODY()

public:
	void AddNewItemToInventoryList(const FString InItemName, const int InItemCount) const;

	FText GetItemName() const { return ItemName->GetText(); }
	FText GetItemCount() const { return ItemCount->GetText(); }

	void SetItemCount(int NewCount) const;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ItemName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> ItemCount;
};
