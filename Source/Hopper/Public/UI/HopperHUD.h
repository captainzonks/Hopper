// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Core/Hopper.h"
#include "Blueprint/UserWidget.h"
#include "Core/HopperPlayerController.h"
#include "HopperHUD.generated.h"

class UHopperListItem;
class UHopperItem;
class AHopperPlayerController;
class UTextBlock;
class UScrollBox;

/**
 * Main HUD for Hopper
 */
UCLASS()
class HOPPER_API UHopperHUD : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

private:
	void UpdateInventoryInformation(bool bAdded, UHopperItem* Item);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UTextBlock> CoinCount;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget, AllowPrivateAccess = true))
	TObjectPtr<UScrollBox> InventoryList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = true, AllowPrivateAccess = true))
	TObjectPtr<AHopperPlayerController> PlayerController;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<UHopperListItem> HopperListItemClass;

	FOnInventoryItemChangedNative InventoryItemChangedNative;
};
