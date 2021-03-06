// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Actors/HopperBaseCharacter.h"
#include "BehaviorTree/BehaviorTreeTypes.h"
#include "Perception/AIPerceptionTypes.h"
#include "HopperAIController.generated.h"

class UAISenseConfig_Sight;
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
	UFUNCTION()
	void OnActorSensed(AActor* Actor, FAIStimulus Stimulus);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI", meta = (AllowPrivateAccess = true))
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta = (AllowPrivateAccess = true))
	TSubclassOf<AHopperBaseCharacter> PlayerClass;
	
	TObjectPtr<UAISenseConfig_Sight> SenseConfig_Sight;
	TObjectPtr<UBlackboardData> BlackboardData;
};
