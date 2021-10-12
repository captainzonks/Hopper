// © 2021, Matthew Barham. All rights reserved.


#include "Core/Abilities/HopperAbilityTask_MultiWaitEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UHopperAbilityTask_MultiWaitEvent* UHopperAbilityTask_MultiWaitEvent::MultiWaitEvent(
	UGameplayAbility* OwningAbility, FGameplayTag SuccessTag, FGameplayTag FailedTag, AActor* OptionalExternalTarget,
	bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UHopperAbilityTask_MultiWaitEvent* MyObj = NewAbilityTask<UHopperAbilityTask_MultiWaitEvent>(OwningAbility);
	MyObj->SuccessTag = SuccessTag;
	MyObj->FailedTag = FailedTag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;

	return MyObj;
}

void UHopperAbilityTask_MultiWaitEvent::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

TWeakObjectPtr<UAbilitySystemComponent> UHopperAbilityTask_MultiWaitEvent::GetTargetASC() const
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return TWeakObjectPtr<UAbilitySystemComponent>(AbilitySystemComponent);
}

void UHopperAbilityTask_MultiWaitEvent::Activate()
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetTargetASC();
	if (ASC.Get())
	{
		if (OnlyMatchExact)
		{
			SuccessHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).AddUObject(
				this, &UHopperAbilityTask_MultiWaitEvent::SuccessEventCallback);
			FailedHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).AddUObject(
				this, &UHopperAbilityTask_MultiWaitEvent::FailedEventCallback);
		}
		else
		{
			SuccessHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag),
			                                                          FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				                                                          this,
				                                                          &UHopperAbilityTask_MultiWaitEvent::SuccessEventContainerCallback));
			FailedHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag),
			                                                         FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				                                                         this,
				                                                         &UHopperAbilityTask_MultiWaitEvent::FailedEventContainerCallback));
		}
	}

	Super::Activate();
}

void UHopperAbilityTask_MultiWaitEvent::SuccessEventCallback(const FGameplayEventData* Payload)
{
	SuccessEventContainerCallback(SuccessTag, Payload);
}

void UHopperAbilityTask_MultiWaitEvent::FailedEventCallback(const FGameplayEventData* Payload)
{
	FailedEventContainerCallback(FailedTag, Payload);
}

void UHopperAbilityTask_MultiWaitEvent::SuccessEventContainerCallback(FGameplayTag MatchingTag,
                                                                      const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		EventSuccess.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UHopperAbilityTask_MultiWaitEvent::FailedEventContainerCallback(FGameplayTag MatchingTag,
                                                                     const FGameplayEventData* Payload)
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		FGameplayEventData TempPayload = *Payload;
		TempPayload.EventTag = MatchingTag;
		EventFailed.Broadcast(MoveTemp(TempPayload));
	}
	if (OnlyTriggerOnce)
	{
		EndTask();
	}
}

void UHopperAbilityTask_MultiWaitEvent::OnDestroy(bool bAbilityEnding)
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetTargetASC();
	if (ASC.Get() && SuccessHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).Remove(SuccessHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag), SuccessHandle);
		}
	}

	if (ASC.Get() && FailedHandle.IsValid())
	{
		if (OnlyMatchExact)
		{
			ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).Remove(FailedHandle);
		}
		else
		{
			ASC->RemoveGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag), FailedHandle);
		}
	}

	Super::OnDestroy(bAbilityEnding);
}
