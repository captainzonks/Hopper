// © 2021, Matthew Barham. All rights reserved.

#pragma once
#include "Core/HopperData.h"

#include "HopperAttackInterface.generated.h"

UINTERFACE(MinimalAPI, Blueprintable)
class UHopperAttackInterface : public UInterface
{
	GENERATED_BODY()
};

class IHopperAttackInterface
{    
	GENERATED_BODY()

public:
	virtual void Attack(const FHopperAttack& Attack) = 0;
};
