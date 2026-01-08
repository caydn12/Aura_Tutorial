// Copyright Kickback Studio


#include "Game/AuraGameInstance.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"
#include "EngineUtils.h"
#include "Interaction/SaveInterface.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	SoftLoadedMaps.Add(DefaultMapName, DefaultMap);
}

void UAuraGameInstance::SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel)
{
	DeleteSaveSlot(SaveSlotViewModel->GetSaveSlotName());
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass);

	ULoadMenuSaveGame* LoadMenuSaveGameObject = Cast<ULoadMenuSaveGame>(SaveGameObject);

	LoadMenuSaveGameObject->PlayerName = SaveSlotViewModel->GetPlayerName();
	LoadMenuSaveGameObject->MapName = SaveSlotViewModel->GetMapName();
	LoadMenuSaveGameObject->PlayerStartTag = SaveSlotViewModel->GetPlayerStartTag();
	LoadMenuSaveGameObject->PlayerLevel = SaveSlotViewModel->GetPlayerLevel();

	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGameObject, SaveSlotViewModel->GetSaveSlotName(), 0);
}

void UAuraGameInstance::SaveInGameData(ULoadMenuSaveGame* SaveDataObject)
{
	PlayerStartTag = SaveDataObject->PlayerStartTag;

	UGameplayStatics::SaveGameToSlot(SaveDataObject, SaveSlotName, 0);
}

ULoadMenuSaveGame* UAuraGameInstance::GetSaveSlotData(const FString& SlotName)
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

void UAuraGameInstance::SaveWorldState(UWorld* World)
{
	FString WorldName = World->GetMapName();
	WorldName.RemoveFromStart(World->StreamingLevelsPrefix);

	if (ULoadMenuSaveGame* SaveGame = GetSaveSlotData(SaveSlotName))
	{
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

void UAuraGameInstance::TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot)
{
	const FString SlotName = SaveSlot->GetSaveSlotName();

	UGameplayStatics::OpenLevelBySoftObjectPtr(SaveSlot, SoftLoadedMaps.FindChecked(SaveSlot->GetMapName()));
}
