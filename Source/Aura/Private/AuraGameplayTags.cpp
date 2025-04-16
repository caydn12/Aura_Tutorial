// Copyright Kickback Studio


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

// Must create static member instance in cpp file
FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	GameplayTags.Attributes_Secondary_Armor = UGameplayTagsManager::Get().AddNativeGameplayTag(FName("Attributes.Secondary.Armor"), FString("Reduces Damage Taken. Improves Block Chance."));
	
}
