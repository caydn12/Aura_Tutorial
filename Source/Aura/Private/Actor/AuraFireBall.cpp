// Copyright Kickback Studio


#include "Actor/AuraFireBall.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"
#include "GameplayCueManager.h"
#include "AuraGameplayTags.h"

AAuraFireBall::AAuraFireBall()
{
	ProjectileMovement->PrimaryComponentTick.bCanEverTick = false;
	ProjectileMovement->SetAutoActivate(false);
}

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValidOverlap(OtherActor))
	{
		if (!HitActors.Contains(OtherActor) && HasAuthority())
		{
			// Apply effect only on server. Effect will modify replicated data
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
				DamageEffectParams.DeathImpulse = DeathImpulse;

				DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
				UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);

				HitActors.AddUnique(OtherActor);
			}
		}
	}
}

void AAuraFireBall::OnHit()
{
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();

		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bImpacted = true;
}

void AAuraFireBall::EmptyHitActorsArray()
{
	HitActors.Empty();
}
