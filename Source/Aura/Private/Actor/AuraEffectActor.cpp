// Copyright Kickback Studio


#include "Actor/AuraEffectActor.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Kismet/KismetMathLibrary.h"

AAuraEffectActor::AAuraEffectActor()
{
 	PrimaryActorTick.bCanEverTick = true;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bSinusoidalMovement)
	{
		RunningTime += DeltaTime;

		const float SinePeriod = ((2 * PI) / SinePeriodConstant);

		if (RunningTime > SinePeriod)
		{
			RunningTime = 0.f;
		}
	}
	HandleSinusoidalMovement(DeltaTime);
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AAuraEffectActor::StartSinusoidalMovement()
{
	RunningTime = 0.f;
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass, const EEffectRemovalPolicy RemovalPolicy)
{
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	if (!bIsEnemy || (bIsEnemy && bApplyEffectsToEnemies))
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

		if (TargetASC)
		{
			check(GameplayEffectClass);
			FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
			EffectContextHandle.AddSourceObject(this);

			const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);

			const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

			const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
			if (bIsInfinite && RemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
			{
				ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
			}
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
					if (TargetASC == HandlePair.Value && HasAuthority())
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
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	const bool bIsPlayer = TargetActor->ActorHasTag(FName("Player"));
	if (!bIsEnemy || (bIsEnemy && bApplyEffectsToEnemies))
	{
		if (bIsPlayer)
		{
			EvaluateEffectsForApplication(TargetActor, InstantEffects, EEffectApplicationPolicy::ApplyOnOverlap);
			EvaluateEffectsForApplication(TargetActor, DurationEffects, EEffectApplicationPolicy::ApplyOnOverlap);
			EvaluateEffectsForApplication(TargetActor, InfiniteEffects, EEffectApplicationPolicy::ApplyOnOverlap);

			if (bDestroyOnEffectApplication)
			{
				Destroy();
			}
		}
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	const bool bIsEnemy = TargetActor->ActorHasTag(FName("Enemy"));
	const bool bIsPlayer = TargetActor->ActorHasTag(FName("Player"));

	if (!bIsEnemy || (bIsEnemy && bApplyEffectsToEnemies))
	{
		if (bIsPlayer)
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
	}
}

void AAuraEffectActor::HandleSinusoidalMovement(float DeltaTime)
{
	if (bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaTime * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}

	if (bSinusoidalMovement)
	{
		const float DeltaHeight = SineAmplitude * FMath::Sin(RunningTime * SinePeriodConstant);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, DeltaHeight);
	}
}
