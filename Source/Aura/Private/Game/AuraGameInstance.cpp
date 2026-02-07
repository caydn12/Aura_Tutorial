// Copyright Kickback Studio


#include "Game/AuraGameInstance.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"
#include "EngineUtils.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "AuraLogChannels.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	SoftLoadedMaps.Add(DefaultMapName, DefaultMap);

	ResetLevelClearedData();
}

void UAuraGameInstance::SetCompletedGoalpoints(const int32 NewCompletedGoalpoints)
{
	if (CompletedGoalpoints != NewCompletedGoalpoints)
	{
		CompletedGoalpoints = NewCompletedGoalpoints;
		OnCompletedGoalpointsChangedGIDelegate.Broadcast(CompletedGoalpoints);
	}
}

void UAuraGameInstance::SetLevelCleared(ELevelID LevelID, bool bCleared)
{
	if (bCleared != IsLevelCleared(LevelID))
	{
		LevelClearedMap[LevelID] = bCleared;
		OnLevelClearedChangedDelegate.Broadcast(LevelClearedMap[LevelID]);

		if (AreAllGoalpointsCompleted())
		{
			OnGoalpointsCompletedDelegate.Broadcast();
		}
	}
}

bool UAuraGameInstance::AreAllGoalpointsCompleted() const
{
	bool bAllCleared = true;
	for (const auto& LevelClearedPair : LevelClearedMap)
	{
		if (!LevelClearedPair.Value)
		{
			bAllCleared = false;
			break;
		}
	}
	return bAllCleared;
}

void UAuraGameInstance::ResetLevelClearedData()
{
	LevelClearedMap.Empty();
	LevelClearedMap.Add(ELevelID::Level_01, false);
	LevelClearedMap.Add(ELevelID::Level_02, false);
	LevelClearedMap.Add(ELevelID::Level_03, false);
	LevelClearedMap.Add(ELevelID::Level_04, false);
	LevelClearedMap.Add(ELevelID::Level_05, false);
	LevelClearedMap.Add(ELevelID::Level_06, false);
	LevelClearedMap.Add(ELevelID::Level_07, false);
	LevelClearedMap.Add(ELevelID::Level_08, false);
}

void UAuraGameInstance::SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel)
{
	DeleteSaveSlot(SaveSlotViewModel->GetSaveSlotName());
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass);

	ULoadMenuSaveGame* LoadMenuSaveGameObject = Cast<ULoadMenuSaveGame>(SaveGameObject);

	LoadMenuSaveGameObject->PlayerName = SaveSlotViewModel->GetPlayerName();
	LoadMenuSaveGameObject->MapName = SaveSlotViewModel->GetMapName();
	LoadMenuSaveGameObject->MapAssetName = SaveSlotViewModel->MapAssetName;
	LoadMenuSaveGameObject->PlayerStartTag = SaveSlotViewModel->GetPlayerStartTag();
	LoadMenuSaveGameObject->PlayerLevel = SaveSlotViewModel->GetPlayerLevel();
	LoadMenuSaveGameObject->CompletedGoalpoints = SaveSlotViewModel->GetCompletedGoalpoints();

	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGameObject, SaveSlotViewModel->GetSaveSlotName(), 0);
}

void UAuraGameInstance::SaveInGameData(ULoadMenuSaveGame* SaveDataObject)
{
	// This looks weird but is correct logic
	PlayerStartTag = SaveDataObject->PlayerStartTag;

	// Getting the game instance current count for saving
	SaveDataObject->CompletedGoalpoints = CompletedGoalpoints;
	SaveDataObject->UpdateLevelClearedMap(LevelClearedMap);

	UGameplayStatics::SaveGameToSlot(SaveDataObject, SaveSlotName, 0);
}

ULoadMenuSaveGame* UAuraGameInstance::GetSaveSlotData(const FString& SlotName) const
{
	ULoadMenuSaveGame* LoadMenuSaveObject = nullptr;

	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		LoadMenuSaveObject = Cast<ULoadMenuSaveGame>(UGameplayStatics::LoadGameFromSlot(SlotName, 0));
	}
	else
	{
		LoadMenuSaveObject = Cast<ULoadMenuSaveGame>(UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass));
	}
	return LoadMenuSaveObject;
}

ULoadMenuSaveGame* UAuraGameInstance::GetInGameSaveData()
{
	return GetSaveSlotData(SaveSlotName);
}

void UAuraGameInstance::DeleteSaveSlot(const FString& SlotName)
{
	// UserIndex would be something like a steam ID. Not relevant single player.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}
}

void UAuraGameInstance::SaveWorldState(UWorld* World, const FString& InDestinationMapAssetName) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (ULoadMenuSaveGame* SaveGame = GetSaveSlotData(SaveSlotName))
	{
		if (InDestinationMapAssetName != FString(""))
		{
			SaveGame->MapAssetName = InDestinationMapAssetName;
			SaveGame->MapName = GetMapNameFromMapAssetName(InDestinationMapAssetName);
		}

		if (!SaveGame->DoesSavedMapExist(WorldName))
		{
			FSavedMap NewSavedMap;
			NewSavedMap.MapAssetName = WorldName;
			SaveGame->SavedMaps.Add(NewSavedMap);
		}

		FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
		SavedMap.SavedActors.Empty();

		for (FActorIterator It(World); It; ++It)
		{
			AActor* Actor = *It;
			if (Actor && Actor->Implements<USaveInterface>())
			{
				FSavedActor FoundSavedActor;
				FoundSavedActor.ActorName = Actor->GetFName();
				FoundSavedActor.Transform = Actor->GetActorTransform();

				FMemoryWriter MemoryWriter(FoundSavedActor.Bytes);
				FObjectAndNameAsStringProxyArchive Archive(MemoryWriter, true);
				Archive.ArIsSaveGame = true;
				Actor->Serialize(Archive);

				SavedMap.SavedActors.AddUnique(FoundSavedActor);
			}
		}

		for (FSavedMap& MapToReplace : SaveGame->SavedMaps)
		{
			if (MapToReplace.MapAssetName == WorldName)
			{
				MapToReplace = SavedMap;
				break;
			}
		}

		UGameplayStatics::SaveGameToSlot(SaveGame, SaveSlotName, 0);
	}
}

void UAuraGameInstance::LoadWorldState(UWorld* World) const
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (UGameplayStatics::DoesSaveGameExist(SaveSlotName, 0))
	{
		ULoadMenuSaveGame* SaveGame = GetSaveSlotData(SaveSlotName);
		if (!SaveGame)
		{
			UE_LOG(LogAura, Error, TEXT("Failed to load slot"));
		}
		else
		{
			FSavedMap SavedMap = SaveGame->GetSavedMapWithMapName(WorldName);
			for (const FSavedActor& SavedActor : SavedMap.SavedActors)
			{
				for (FActorIterator It(World); It; ++It)
				{
					AActor* Actor = *It;
					if (Actor && Actor->Implements<USaveInterface>() && SavedActor.ActorName == Actor->GetFName())
					{
						if (ISaveInterface::Execute_ShouldLoadTransform(Actor))
						{
							Actor->SetActorTransform(SavedActor.Transform);
						}
						FMemoryReader MemoryReader(SavedActor.Bytes);
						FObjectAndNameAsStringProxyArchive Archive(MemoryReader, true);
						Archive.ArIsSaveGame = true;
						Actor->Serialize(Archive); // converts binary bytes back into variables

						ISaveInterface::Execute_LoadActor(Actor);

						break;
					}
				}
			}
		}
	}
}

void UAuraGameInstance::TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot)
{
	const FString SlotName = SaveSlot->GetSaveSlotName();

	UGameplayStatics::OpenLevelBySoftObjectPtr(SaveSlot, SoftLoadedMaps.FindChecked(SaveSlot->GetMapName()));
}

FString UAuraGameInstance::GetMapNameFromMapAssetName(const FString& MapAssetName) const
{
	FString FoundMapName = FString();
	for (auto& Map : SoftLoadedMaps)
	{
		if (Map.Value.ToSoftObjectPath().GetAssetName() == MapAssetName)
		{
			FoundMapName = Map.Key;
		}
	}
	return FoundMapName;
}
