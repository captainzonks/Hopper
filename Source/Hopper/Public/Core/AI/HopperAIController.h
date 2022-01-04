// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "HopperAIController.generated.h"

class UBehaviorTreeComponent;
class UBlackboardComponent;

/**
 * Custom AIController for Hopper game
 */
UCLASS()
class HOPPER_API AHopperAIController : public AAIController
{
	GENERATED_BODY()

public:
	AHopperAIController();

	TObjectPtr<UBehaviorTreeComponent> GetBehaviorTreeComponent() const;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
};
