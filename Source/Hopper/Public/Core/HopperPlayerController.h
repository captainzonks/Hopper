// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "HopperPlayerController.generated.h"

class IHopperAttackInterface;
/**
 * Hopper Player Controller for Gameplay
 */
UCLASS()
class HOPPER_API AHopperPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:
	virtual void SetupInputComponent() override;

};
