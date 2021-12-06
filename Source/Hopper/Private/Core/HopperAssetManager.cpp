// © 2021, Matthew Barham. All rights reserved.

#include "Core/HopperAssetManager.h"
#include "Core/Items/HopperItem.h"
#include "AbilitySystemGlobals.h"

const FPrimaryAssetType UHopperAssetManager::TokenItemType = TEXT("Token");

UHopperAssetManager& UHopperAssetManager::Get()
{
	UHopperAssetManager* This = Cast<UHopperAssetManager>(GEngine->AssetManager);

	if (This)
	{
		return *This;
	}
	else
	{
		UE_LOG(LogHopper, Fatal, TEXT("Invalid AssetManager in DefaultEngine.ini, must be HopperAssetManager!"))
		return *NewObject<UHopperAssetManager>(); // never calls this
	}
}

void UHopperAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	UAbilitySystemGlobals::Get().InitGlobalData();
}

UHopperItem* UHopperAssetManager::ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning)
{
	FSoftObjectPath ItemPath = GetPrimaryAssetPath(PrimaryAssetId);

	// This does a synchronous load and may hitch
	UHopperItem* LoadedItem = Cast<UHopperItem>(ItemPath.TryLoad());

	if (bLogWarning && !LoadedItem)
	{
		UE_LOG(LogHopper, Warning, TEXT("Failed to load item for indentifier %s!"), *PrimaryAssetId.ToString())
	}

	return LoadedItem;
}
