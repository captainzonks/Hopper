// © 2021, Matthew Barham. All rights reserved.


#include "Core/Abilities/AbilityTask_SuccessFailEvent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

UAbilityTask_SuccessFailEvent* UAbilityTask_SuccessFailEvent::WaitSuccessFailEvent(
	UGameplayAbility* OwningAbility, FGameplayTag SuccessTag, FGameplayTag FailedTag, AActor* OptionalExternalTarget,
	bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UAbilityTask_SuccessFailEvent* MyObj = NewAbilityTask<UAbilityTask_SuccessFailEvent>(OwningAbility);
	MyObj->SuccessTag = SuccessTag;
	MyObj->FailedTag = FailedTag;
	MyObj->SetExternalTarget(OptionalExternalTarget);
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;

	return MyObj;
}

void UAbilityTask_SuccessFailEvent::SetExternalTarget(AActor* Actor)
{
	if (Actor)
	{
		UseExternalTarget = true;
		OptionalExternalTarget = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Actor);
	}
}

TWeakObjectPtr<UAbilitySystemComponent> UAbilityTask_SuccessFailEvent::GetTargetASC() const
{
	if (UseExternalTarget)
	{
		return OptionalExternalTarget;
	}

	return TWeakObjectPtr<UAbilitySystemComponent>(AbilitySystemComponent);
}

void UAbilityTask_SuccessFailEvent::Activate()
{
	const TWeakObjectPtr<UAbilitySystemComponent> ASC = GetTargetASC();
	if (ASC.Get())
	{
		if (OnlyMatchExact)
		{
			SuccessHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(SuccessTag).AddUObject(
				this, &UAbilityTask_SuccessFailEvent::SuccessEventCallback);
			FailedHandle = ASC->GenericGameplayEventCallbacks.FindOrAdd(FailedTag).AddUObject(
				this, &UAbilityTask_SuccessFailEvent::FailedEventCallback);
		}
		else
		{
			SuccessHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(SuccessTag),
			                                                          FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				                                                          this,
				                                                          &UAbilityTask_SuccessFailEvent::SuccessEventContainerCallback));
			FailedHandle = ASC->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(FailedTag),
			                                                         FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(
				                                                         this,
				                                                         &UAbilityTask_SuccessFailEvent::FailedEventContainerCallback));
		}
	}

	Super::Activate();
}

void UAbilityTask_SuccessFailEvent::SuccessEventCallback(const FGameplayEventData* Payload)
{
	SuccessEventContainerCallback(SuccessTag, Payload);
}

void UAbilityTask_SuccessFailEvent::FailedEventCallback(const FGameplayEventData* Payload)
{
	FailedEventContainerCallback(FailedTag, Payload);
}

void UAbilityTask_SuccessFailEvent::SuccessEventContainerCallback(FGameplayTag MatchingTag,
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

void UAbilityTask_SuccessFailEvent::FailedEventContainerCallback(FGameplayTag MatchingTag,
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

void UAbilityTask_SuccessFailEvent::OnDestroy(bool bAbilityEnding)
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
