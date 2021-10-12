// © 2021, Matthew Barham. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "HopperAbilityTask_MultiWaitEvent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMultiWaitGameplayEventDelegate, FGameplayEventData, Payload);

/**
 * Custom Ability Task allowing for a Gameplay Event to return a cancel delegate
 */
UCLASS()
class HOPPER_API UHopperAbilityTask_MultiWaitEvent : public UAbilityTask
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FMultiWaitGameplayEventDelegate EventSuccess;

	UPROPERTY(BlueprintAssignable)
	FMultiWaitGameplayEventDelegate EventFailed;

	/**
	* Wait until the specified gameplay tags event's trigger. Allows success or failure.
	* By default this will look at the owner of this ability.
	* OptionalExternalTarget can be set to make this look at another actor's tags for changes.
	* It will keep listening as long as OnlyTriggerOnce = false
	* If OnlyMatchExact = false it will trigger for nested tags
	*/
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks",
		meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UHopperAbilityTask_MultiWaitEvent* MultiWaitEvent(UGameplayAbility* OwningAbility, FGameplayTag SuccessTag,
	                                                         FGameplayTag FailedTag,
	                                                         AActor* OptionalExternalTarget = nullptr,
	                                                         bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	void SetExternalTarget(AActor* Actor);

	TWeakObjectPtr<UAbilitySystemComponent> GetTargetASC() const;

	virtual void Activate() override;

	virtual void SuccessEventCallback(const FGameplayEventData* Payload);
	virtual void FailedEventCallback(const FGameplayEventData* Payload);
	virtual void SuccessEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);
	virtual void FailedEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData* Payload);

	virtual void OnDestroy(bool AbilityEnding) override;

	FGameplayTag SuccessTag;
	FGameplayTag FailedTag;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> OptionalExternalTarget;

	bool UseExternalTarget;
	bool OnlyTriggerOnce;
	bool OnlyMatchExact;

	FDelegateHandle SuccessHandle;
	FDelegateHandle FailedHandle;
};
