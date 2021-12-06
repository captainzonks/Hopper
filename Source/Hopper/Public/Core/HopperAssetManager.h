// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "Core/Hopper.h"
#include "Engine/AssetManager.h"
#include "HopperAssetManager.generated.h"

class UHopperItem;

UCLASS()
class HOPPER_API UHopperAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	/** Constructor and overrides */
	UHopperAssetManager() {}
	virtual void StartInitialLoading() override;

	/** Static types for items */
	static const FPrimaryAssetType TokenItemType;

	/** Returns the current AssetManager object */
	static UHopperAssetManager& Get();

	/**
	 * Synchronously loads a HopperItem subclass, this can hitch but is useful when you cannot wait for an async load
	 * This does not maintain a reference to the item so it will garbage collect if not loaded some other way
	 *
	 * @param PrimaryAssetId The asset identifier to load
	 * @param bLogWarning If true, this will log a warning if the item failed to load
	 */
	UHopperItem* ForceLoadItem(const FPrimaryAssetId& PrimaryAssetId, bool bLogWarning = true);
};
