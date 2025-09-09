// Copyright Kickback Studio


#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetAuraPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{
	GetAuraASC()->AbilityStatusChanged.AddLambda(
		[this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
		{
			if (SelectedAbility.Ability.MatchesTagExact(AbilityTag))
			{
				SelectedAbility.Status = StatusTag;
				bool bEnableSpendPoints = false;
				bool bEnableEquip = false;
				ShouldEnableButtons(StatusTag, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
				SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
			}

			if (AbilityInfo)
			{
				FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfoByTag(AbilityTag);
				Info.StatusTag = StatusTag;
				AbilityInfoDelegate.Broadcast(Info);
			}
		}
	);

	GetAuraPS()->OnSpellPointsChangedDelegate.AddLambda(
		[this](int32 SpellPoints)
		{
			SpellPointsChangedDelegate.Broadcast(SpellPoints);
			CurrentSpellPoints = SpellPoints;

			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(SelectedAbility.Status, CurrentSpellPoints, bEnableSpendPoints, bEnableEquip);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
		}
	);
}

// States:
// Equipped
// Unlocked
// Eligible
// Locked
void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const int32 SpellPoints = GetAuraPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;
	FGameplayAbilitySpec* AbilitySpec = GetAuraASC()->GetSpecFromAbilityTag(AbilityTag);

	bool bTagValid = AbilityTag.IsValid();
	bool bTagNone = AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None);

	// Spec will only be valid if ability is contained in ASC's list of abilities
	bool bSpecValid = AbilitySpec != nullptr;

	// State: Locked -  Has no Tag or Tag is None or Spec is not valid on ASC's ability list
	if (!bTagValid || bTagNone || !bSpecValid)
	{
		AbilityStatus = FAuraGameplayTags::Get().Abilities_Status_Locked;
	}
	else
	{
		AbilityStatus = GetAuraASC()->GetStatusTagFromSpec(*AbilitySpec);
	}

	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus, SpellPoints, bEnableSpendPoints, bEnableEquip);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints, bEnableEquip);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetAuraASC())
	{
		GetAuraASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints, bool& bShouldEnableSpendPointsButton, bool& bShouldEnableEquipButton)
{
	const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
	bShouldEnableSpendPointsButton = false;
	bShouldEnableEquipButton = false;
	if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Locked))
	{
		// Locked - Can't do anything
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
	{
		// Eligible - Can spend points if have enough points
		bShouldEnableSpendPointsButton = SpellPoints > 0;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
	{
		// Unlocked - Can equip or spend points if have enough points
		bShouldEnableSpendPointsButton = SpellPoints > 0;
		bShouldEnableEquipButton = true;
	}
	else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
	{
		// Equipped - Can only spend points if have enough points
		bShouldEnableSpendPointsButton = SpellPoints > 0;
		bShouldEnableEquipButton = true;
	}
}
