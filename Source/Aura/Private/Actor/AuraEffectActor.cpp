// Copyright Kickback Studio


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
 	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass, EEffectRemovalPolicy RemovalPolicy)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (TargetASC)
	{
		check(GameplayEffectClass);
		FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(this);

		const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.0f, EffectContextHandle);

		const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

		const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
		if (bIsInfinite && RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
		{
			ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
		}
	}
}

void AAuraEffectActor::EvaluateEffectsForApplication(AActor* TargetActor, const TArray<FEffectConfiguration>& Effects, const EEffectApplicationPolicy& ApplicationPolicy)
{
	for (const FEffectConfiguration& Effect : Effects)
	{
		if (Effect.EffectApplicationPolicy == ApplicationPolicy)
		{
			ApplyEffectToTarget(TargetActor, Effect.GameplayEffectClass, Effect.EffectRemovalPolicy);
		}
	}
}

void AAuraEffectActor::EvaluateEffectsForRemoval(AActor* TargetActor, const TArray<FEffectConfiguration>& Effects, const EEffectRemovalPolicy& RemovalPolicy)
{
	bool bWasGameplayEffectRemoved = false;
	TArray<FActiveGameplayEffectHandle> HandlesToRemove;
	for (const FEffectConfiguration& Effect : Effects)
	{
		if (Effect.EffectRemovalPolicy == RemovalPolicy)
		{
			UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
			if (IsValid(TargetASC))
			{
				for (TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
				{
					if (TargetASC == HandlePair.Value)
					{
						TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
						HandlesToRemove.Add(HandlePair.Key);
						bWasGameplayEffectRemoved = true;
					}
				}
			}
		}
	}

	for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
	{
		ActiveEffectHandles.FindAndRemoveChecked(Handle);
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	EvaluateEffectsForApplication(TargetActor, InstantEffects, EEffectApplicationPolicy::ApplyOnOverlap);
	EvaluateEffectsForApplication(TargetActor, DurationEffects, EEffectApplicationPolicy::ApplyOnOverlap);
	EvaluateEffectsForApplication(TargetActor, InfiniteEffects, EEffectApplicationPolicy::ApplyOnOverlap);

	if (bDestroyOnEffectApplication)
	{
		Destroy();
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	EvaluateEffectsForApplication(TargetActor, InstantEffects, EEffectApplicationPolicy::ApplyOnEndOverlap);
	EvaluateEffectsForApplication(TargetActor, DurationEffects, EEffectApplicationPolicy::ApplyOnEndOverlap);
	EvaluateEffectsForApplication(TargetActor, InfiniteEffects, EEffectApplicationPolicy::ApplyOnEndOverlap);
	EvaluateEffectsForRemoval(TargetActor, InfiniteEffects, EEffectRemovalPolicy::RemoveOnEndOverlap);

	if (bDestroyOnEffectRemoval)
	{
		Destroy();
	}
}