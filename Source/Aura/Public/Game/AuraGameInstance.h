// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadMenuSaveSlot;
class USaveGame;

UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Saving Data
	void SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadMenuSaveGameClass;

	// Global Music
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "System|Audio")
	void SetGlobalMusicVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "System|Audio")
	float GetGlobalMusicVolume() const { return GlobalMusicVolume; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "System|Audio")
	float GlobalMusicVolume = 0.5f;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
