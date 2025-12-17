// Copyright Kickback Studio


#include "Game/AuraGameInstance.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"

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

	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGameObject, SaveSlotViewModel->GetSaveSlotName(), 0);
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

void UAuraGameInstance::DeleteSaveSlot(const FString& SlotName)
{
	// UserIndex would be something like a steam ID. Not relevant single player.
	if (UGameplayStatics::DoesSaveGameExist(SlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, 0);
	}
}

void UAuraGameInstance::TravelToMap(UMVVM_LoadMenuSaveSlot* SaveSlot)
{
	const FString SlotName = SaveSlot->GetSaveSlotName();

	UGameplayStatics::OpenLevelBySoftObjectPtr(SaveSlot, SoftLoadedMaps.FindChecked(SaveSlot->GetMapName()));
}
