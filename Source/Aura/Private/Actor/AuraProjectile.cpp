// Copyright Kickback Studio


#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/AudioComponent.h"
#include "Aura/Aura.h" // ECC Projectile
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.0f;
	ProjectileMovement->MaxSpeed = 550.0f;
	ProjectileMovement->ProjectileGravityScale = 0.0f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(LifeSpan);

	SetReplicateMovement(true);

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	// On client, after server has handled destroying the object
	// if the client projectile has not yet impacted something, Play the effects on client side
	if (!bImpacted && !HasAuthority())
	{
		OnHit();
	}
	Super::Destroyed();
}

void AAuraProjectile::OnHit()
{
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation(), FRotator::ZeroRotator);

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());

	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}

	bImpacted = true;
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AActor* ProjectileOwner = GetOwner();
	bool bHitSelf = ProjectileOwner == OtherActor;
	if (!bHitSelf)
	{
		if (UAuraAbilitySystemLibrary::IsNotAlly(ProjectileOwner, OtherActor))
		{
			if (!bImpacted)
			{
				OnHit();
			}

			if (HasAuthority())
			{
				// Apply effect only on server. Effect will modify replicated data
				if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
				{
					const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
					DamageEffectParams.DeathImpulse = DeathImpulse;

					const bool bShouldKnockback = FMath::RandRange(1, 100) < DamageEffectParams.KnockbackChance;
					if (bShouldKnockback)
					{
						FRotator Rotation = GetActorRotation();
						Rotation.Pitch = 45.f; // Add vertical component to knockback
						const FVector KnockbackDirection = Rotation.Vector();
						const FVector KnockbackForce = KnockbackDirection * DamageEffectParams.KnockbackForceMagnitude;
						DamageEffectParams.KnockbackForce = KnockbackForce;
					}
					

					DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
					UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
				}

				// If on server, Destroy the projectile on overlap with other actor
				Destroy();
			}
			else
			{
				// If on client, set bImpacted to true because only the server destroys the object
				bImpacted = true;
			}
		}
	}
}