// Copyright Kickback Studio


#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "UI/HUD/AuraHUD.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "NiagaraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"

AAuraCharacter::AAuraCharacter()
{
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->bDoCollisionTest = false;
	CameraBoom->bEnableCameraLag = true;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	LevelUpNiagaraComponent->SetAutoActivate(false);

	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 400.0f, 0.0f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// Init Ability Actor info for the server
	InitAbilityActorInfo();
	LoadProgress();

	if (AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>())
	{
		AuraPlayerState->InitializeDebuffTagsToDebuffEffects();
		AuraPlayerState->InitializeSiphonTagsToSiphonEffects();
	}
}

void AAuraCharacter::LoadProgress()
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (ULoadMenuSaveGame* SaveData = AuraGameInstance->GetInGameSaveData())
		{
			if (SaveData->bFirstTimePlaying)
			{
				InitializeDefaultAttributes();

				// Only called on server. Replicated to clients.
				AddCharacterAbilities();
			}
			else
			{
				// Load Player State Data
				if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
				{
					AuraPlayerState->SetLevel(SaveData->PlayerLevel);
					AuraPlayerState->SetXP(SaveData->PlayerXP);
					AuraPlayerState->SetAttributePoints(SaveData->AttributePoints);
					AuraPlayerState->SetSpellPoints(SaveData->SpellPoints);
				}

				// Load Attributes
				UAuraAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
				ApplyEffectToSelf(DefaultSecondaryAttributes, SaveData->PlayerLevel);
				ApplyEffectToSelf(DefaultVitalAttributes, SaveData->PlayerLevel);

				// Load Abilities
				if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
				{
					AuraASC->AddCharacterAbilitiesFromSaveData(SaveData);
					AuraASC->UpdateAbilityStatuses(SaveData->PlayerLevel);
				}
			}
		}
	}
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Init Ability Actor info for the client

	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToXP(InXP);
}

void AAuraCharacter::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToLevel(InPlayerLevel);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		AuraASC->UpdateAbilityStatuses(AuraPlayerState->GetPlayerLevel());
	}
}

void AAuraCharacter::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToAttributePoints(InAttributePoints);
}

void AAuraCharacter::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->AddToSpellPoints(InSpellPoints);
}

int32 AAuraCharacter::GetXP_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerXP();
}

int32 AAuraCharacter::GetAttributePointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 AAuraCharacter::GetSpellPointsReward_Implementation(int32 Level) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void AAuraCharacter::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}

int32 AAuraCharacter::FindLevelForXP_Implementation(int32 InXP) const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->LevelUpInfo->FindLevelForXP(InXP);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetSpellPoints();
}

void AAuraCharacter::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial, float Radius)
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->ShowMagicCircle(DecalMaterial, Radius);
		AuraPlayerController->bShowMouseCursor = false;
	}
}

void AAuraCharacter::HideMagicCircle_Implementation()
{
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AuraPlayerController->HideMagicCircle();
		AuraPlayerController->bShowMouseCursor = true;
	}
}

void AAuraCharacter::SaveProgress_Implementation(const FName& CheckpointTag)
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		ULoadMenuSaveGame* SaveData = AuraGameInstance->GetInGameSaveData();
		if (SaveData)
		{
			SaveData->PlayerStartTag = CheckpointTag;

			if (AAuraPlayerState* AuraPlayerState = Cast<AAuraPlayerState>(GetPlayerState()))
			{
				SaveData->PlayerLevel = AuraPlayerState->GetPlayerLevel();
				SaveData->PlayerXP = AuraPlayerState->GetPlayerXP();
				SaveData->SpellPoints = AuraPlayerState->GetSpellPoints();
				SaveData->AttributePoints = AuraPlayerState->GetAttributePoints();
			}

			SaveData->Strength = UAuraAttributeSet::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Intelligence = UAuraAttributeSet::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Resilience = UAuraAttributeSet::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
			SaveData->Vigor = UAuraAttributeSet::GetVigorAttribute().GetNumericValue(GetAttributeSet());

			SaveData->bFirstTimePlaying = false;

			if (HasAuthority())
			{
				UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent);
				FForEachAbility SaveAbilityDelegate;
				SaveAbilityDelegate.BindLambda(
					[this, AuraASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
					{
						const FGameplayTag AbilityTag = AuraASC->GetAbilityTagFromSpec(AbilitySpec);
						UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(this);
						FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);

						FSavedAbility SavedAbility;
						SavedAbility.GameplayAbilityClass = Info.AbilityClass;
						SavedAbility.AbilityTag = AbilityTag;
						SavedAbility.AbilityLevel = AbilitySpec.Level;
						SavedAbility.AbilitySlot = AuraASC->GetInputTagFromAbilityTag(AbilityTag);
						SavedAbility.AbilityStatus = AuraASC->GetStatusFromAbilityTag(AbilityTag);
						SavedAbility.AbilityType = Info.AbilityTypeTag;

						SaveData->SavedAbilities.Add(SavedAbility);
					}
				);
				// Clear existing saved abilities before saving current ones
				SaveData->SavedAbilities.Empty();
				AuraASC->ForEachAbility(SaveAbilityDelegate);

				AuraGameInstance->SaveInGameData(SaveData);
			}
		}
	}
}

int32 AAuraCharacter::GetCharacterLevel_Implementation() const
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::OnRep_Burned()
{
	if (bIsBurned)
	{
		if (!BurnDebuffComponent->IsActive())
		{
			BurnDebuffComponent->Activate();
		}
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent))
	{
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_CursorTrace);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputHeld);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputPressed);
		BlockedTags.AddTag(FAuraGameplayTags::Get().Player_Block_InputReleased);

		if (bIsStunned)
		{
			AuraASC->AddLooseGameplayTags(BlockedTags);
			if (!StunDebuffComponent->IsActive())
			{
				StunDebuffComponent->Activate();
			}
		}
		else
		{
			AuraASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_PhysicalDebuff()
{
	if (bIsPhysicallyDebuffed)
	{
		if (!PhysicalDebuffComponent->IsActive())
		{
			PhysicalDebuffComponent->Activate();
		}
	}
	else
	{
		PhysicalDebuffComponent->Deactivate();
	}
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();

	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	OnASCRegistered.Broadcast(AbilitySystemComponent);

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Effect_Burn,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AAuraCharacter::BurnTagChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Effect_Stun,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AAuraCharacter::StunTagChanged);

	AbilitySystemComponent->RegisterGameplayTagEvent(
		FAuraGameplayTags::Get().Debuff_Effect_Physical,
		EGameplayTagEventType::NewOrRemoved
	).AddUObject(this, &AAuraCharacter::PhysicalTagChanged);

	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
}

void AAuraCharacter::MulticastLevelUpParticles_Implementation() const
{
	if (IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = TopDownCameraComponent->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation();

		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);

		LevelUpNiagaraComponent->Activate(true);
	}
}