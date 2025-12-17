// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;
class UMVVM_LoadMenuSaveSlot;
class USaveGame;
class ULoadMenuSaveGame;

UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	// Begin Play Equivalent
	virtual void Init() override;

	// Saving Data
	void SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel);

	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName);

	static void DeleteSaveSlot(const FString& SlotName);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadMenuSaveGameClass;

	// Map Data

	void TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot);

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> SoftLoadedMaps;

	// Global Music
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "System|Audio")
	void SetGlobalMusicVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "System|Audio")
	float GetGlobalMusicVolume() const { return GlobalMusicVolume; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "System|Audio")
	float GlobalMusicVolume = 0.5f;

	// Character Class & Ability Info

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
