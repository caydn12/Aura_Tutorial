// Copyright Kickback Studio

#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h" // ECC Projectile
#include "AuraGameplayTags.h"
#include "kismet/GameplayStatics.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "net/UnrealNetwork.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	BurnDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("BurnDebuffComponent"));
	BurnDebuffComponent->SetupAttachment(GetRootComponent());
	BurnDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Effect_Burn;

	StunDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("StunDebuffComponent"));
	StunDebuffComponent->SetupAttachment(GetRootComponent());
	StunDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Effect_Stun;

	PhysicalDebuffComponent = CreateDefaultSubobject<UDebuffNiagaraComponent>(TEXT("PhysicalDebuffComponent"));
	PhysicalDebuffComponent->SetupAttachment(GetRootComponent());
	PhysicalDebuffComponent->DebuffTag = FAuraGameplayTags::Get().Debuff_Effect_Physical;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Ensure we get correct weapon socket location by the time an AnimNotify is fired, Client issue.
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

	EffectAttachComponent = CreateDefaultSubobject<USceneComponent>("EffectAttachPoint");
	EffectAttachComponent->SetupAttachment(GetRootComponent());
	EffectAttachComponent->SetUsingAbsoluteRotation(true);
	EffectAttachComponent->SetWorldRotation(FRotator::ZeroRotator);

	HaloOfProtectionNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("HaloOfProtectionComponent");
	HaloOfProtectionNiagaraComponent->SetupAttachment(EffectAttachComponent);

	LifeSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("LifeSiphonComponent");
	LifeSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	ManaSiphonNiagaraComponent = CreateDefaultSubobject<UPassiveNiagaraComponent>("ManaSiphonComponent");
	ManaSiphonNiagaraComponent->SetupAttachment(EffectAttachComponent);

	GetCapsuleComponent()->SetReceivesDecals(false);
	GetMesh()->SetReceivesDecals(false);
	Weapon->SetReceivesDecals(false);
}

void AAuraCharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraCharacterBase, bIsBurned);
	DOREPLIFETIME(AAuraCharacterBase, bIsStunned);
	DOREPLIFETIME(AAuraCharacterBase, bIsBeingShocked);
	DOREPLIFETIME(AAuraCharacterBase, bIsPhysicallyDebuffed);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitAbilityActorInfo()
{

}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& SocketTag)
{
	FVector SocketLocation = GetActorLocation();
	if (SocketTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Weapon) && IsValid(Weapon))
	{
		SocketLocation = Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	else if (SocketTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_LeftHand))
	{
		SocketLocation = GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	else if (SocketTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_RightHand))
	{
		SocketLocation = GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	else if (SocketTag.MatchesTagExact(FAuraGameplayTags::Get().CombatSocket_Tail))
	{
		SocketLocation = GetMesh()->GetSocketLocation(TailSocketName);
	}
	return SocketLocation;
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> AAuraCharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontages;
}

UNiagaraSystem* AAuraCharacterBase::GetBloodEffect_Implementation() const
{
	return BloodEffect;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	FTaggedMontage TM;
	for (FTaggedMontage TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
		{
			TM = TaggedMontage;
		}	
	}
	return TM;
}

int32 AAuraCharacterBase::GetMinionCount_Implementation() const
{
	return MinionCount;
}

int32 AAuraCharacterBase::GetMinionSpawnLowerThreshold_Implementation() const
{
	return MinionSpawnLowerThreshold;
}

void AAuraCharacterBase::IncreaseMinionCount_Implementation(int32 Amount)
{
	MinionCount += Amount;
}

ECharacterClass AAuraCharacterBase::GetCharacterClass_Implementation()
{
	return CharacterClass;
}

FOnASCRegistered& AAuraCharacterBase::GetOnASCRegisteredDelegate()
{
	return OnASCRegistered;
}

FOnDeath& AAuraCharacterBase::GetOnDeathDelegate()
{
	return OnDeath;
}

USkeletalMeshComponent* AAuraCharacterBase::GetWeapon_Implementation()
{
	return Weapon;
}

bool AAuraCharacterBase::IsBeingShocked_Implementation() const
{
	return bIsBeingShocked;
}

void AAuraCharacterBase::SetIsBeingShocked_Implementation(bool bInBeingShocked)
{
	bIsBeingShocked = bInBeingShocked;
}

FVector AAuraCharacterBase::GetCharacterLocationOnFloor_Implementation() const
{
	FVector FoundLocation = GetActorLocation();
	if (GetCharacterMovement() && 
		GetCharacterMovement()->CurrentFloor.IsWalkableFloor() &&
		GetCharacterMovement()->CurrentFloor.HitResult.bBlockingHit)
	{
		FoundLocation = GetCharacterMovement()->CurrentFloor.HitResult.ImpactPoint;
	}
	else
	{
		UWorld* World = GetWorld();
		if (World)
		{
			const FVector Start = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);
			const FVector End = GetActorLocation() - FVector(0.0f, 0.0f, 500.0f);

			FHitResult Hit;
			FCollisionQueryParams Params(SCENE_QUERY_STAT(GetCharacterLocationOnFloor), false);
			Params.AddIgnoredActor(this);

			if (const USkeletalMeshComponent* SkeletalMesh = GetMesh())
			{
				Params.AddIgnoredComponent(SkeletalMesh);
			}
			if (const UCapsuleComponent* Capsule = GetCapsuleComponent())
			{
				Params.AddIgnoredComponent(Capsule);
			}

			if (World->LineTraceSingleByChannel(Hit, Start, End, ECC_Visibility, Params))
			{
				FoundLocation = Hit.ImpactPoint;
			}
		}
	}

	return FoundLocation;
}

void AAuraCharacterBase::OnRep_Burned()
{

}

void AAuraCharacterBase::OnRep_Stunned()
{

}

void AAuraCharacterBase::OnRep_PhysicalDebuff()
{

}

void AAuraCharacterBase::BurnTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsBurned = NewCount > 0;
}

void AAuraCharacterBase::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsStunned = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bIsStunned ? 0.0f : BaseWalkSpeed;
}

void AAuraCharacterBase::PhysicalTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bIsPhysicallyDebuffed = NewCount > 0;
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);

	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);

	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, level, ContextHandle);

	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(
		*SpecHandle.Data.Get(),
		GetAbilitySystemComponent()
	);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.0f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.0f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);

	if (HasAuthority())
	{
		AuraASC->AddCharacterAbilities(StartupAbilities);
		AuraASC->AddCharacterPassiveAbilities(StartupPassiveAbilities);
	}
}

void AAuraCharacterBase::Dissolve()
{
	if (IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeline(DynamicMatInst);
	}
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeline(DynamicMatInst);
	}
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation(const FVector& DeathImpulse)
{
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());

	// AI Recommended fix to prevent falling through the floor on death
	// Also fixes input causing movement of the staff when player is dead
	// Also fixes Aura running sound loop to not continue after death
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	// End AI Fix

	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	Weapon->AddImpulse(DeathImpulse* 0.1f, NAME_None, true);

	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	GetMesh()->AddImpulse(DeathImpulse, NAME_None, true);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
	BurnDebuffComponent->Deactivate();
	StunDebuffComponent->Deactivate();
	PhysicalDebuffComponent->Deactivate();
	OnDeath.Broadcast(this);
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die(const FVector& DeathImpulse)
{
	// Server call
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// Server and Client calls
	MulticastHandleDeath(DeathImpulse);
}

void AAuraCharacterBase::Knockback(const FVector& KnockbackForce)
{
	LaunchCharacter(KnockbackForce, false, false);
}
