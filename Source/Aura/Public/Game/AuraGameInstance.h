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

	// Player Start Data
	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString SaveSlotName = FString();

	// Tutorial uses this slot index but I don't track slot index in my system
	//UPROPERTY()
	//int32 SaveSlotIndex = 0;

	// Saving Data
	void SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel);
	void SaveInGameData(ULoadMenuSaveGame* SaveDataObject);

	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName) const;
	ULoadMenuSaveGame* GetInGameSaveData();

	static void DeleteSaveSlot(const FString& SlotName);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadMenuSaveGameClass;

	// Map Data

	void SaveWorldState(UWorld* World) const;
	void LoadWorldState(UWorld* World) const;

	void TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot);

	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> SoftLoadedMaps;

	// Global Audio
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
