// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnCompletedGoalpointsChangedGISignature, int32 /*NewCompletedGoalpoints*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelClearedChangedSignature, bool /*bCleared*/);

UENUM(BlueprintType)
enum class ELevelID : uint8
{
	Level_01,
	Level_02,
	Level_03,
	Level_04,
	Level_05,
	Level_06,
	Level_07,
	Level_08,
	Level_None
};

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

	// Goalpoints

	void SetCompletedGoalpoints(const int32 NewCompletedGoalpoints);
	int32 GetCompletedGoalpoints() const { return CompletedGoalpoints; }

	UFUNCTION()
	void SetLevelCleared(ELevelID LevelID, bool bCleared);

	UFUNCTION()
	bool IsLevelCleared(ELevelID LevelID) const { return LevelClearedMap.Contains(LevelID) ? LevelClearedMap[LevelID] : false; }

	UFUNCTION()
	void ResetLevelClearedData();

	FOnLevelClearedChangedSignature OnLevelClearedChangedDelegate;

	// Player Start Data
	UPROPERTY()
	FName PlayerStartTag = FName();

	UPROPERTY()
	FString SaveSlotName = FString();

	// Saving Data
	void SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel);
	void SaveInGameData(ULoadMenuSaveGame* SaveDataObject);

	ULoadMenuSaveGame* GetSaveSlotData(const FString& SlotName) const;
	ULoadMenuSaveGame* GetInGameSaveData();

	static void DeleteSaveSlot(const FString& SlotName);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadMenuSaveGameClass;

	// Map Data

	void SaveWorldState(UWorld* World, const FString& InDestinationMapAssetName = FString()) const;
	void LoadWorldState(UWorld* World) const;

	void TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot);

	FString GetMapNameFromMapAssetName(const FString& MapAssetName) const;

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

	FOnCompletedGoalpointsChangedGISignature OnCompletedGoalpointsChangedGIDelegate;

	// Goal point objectives
	UPROPERTY(BlueprintReadOnly)
	int32 CompletedGoalpoints = 0;

	UPROPERTY()
	TMap<ELevelID, bool> LevelClearedMap;
};
