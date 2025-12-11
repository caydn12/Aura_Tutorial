// Copyright Kickback Studio


#include "Game/AuraGameInstance.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"

void UAuraGameInstance::SaveSlotData(UMVVM_LoadMenuSaveSlot* SaveSlotViewModel)
{
	// UserIndex would be something like a steam ID. Not relevant single player.
	if (UGameplayStatics::DoesSaveGameExist(SaveSlotViewModel->SaveSlotName, 0))
	{
		UGameplayStatics::DeleteGameInSlot(SaveSlotViewModel->SaveSlotName, 0);
	}
	USaveGame* SaveGameObject = UGameplayStatics::CreateSaveGameObject(LoadMenuSaveGameClass);

	ULoadMenuSaveGame* LoadMenuSaveGameObject = Cast<ULoadMenuSaveGame>(SaveGameObject);

	LoadMenuSaveGameObject->PlayerName = SaveSlotViewModel->PlayerName;

	UGameplayStatics::SaveGameToSlot(LoadMenuSaveGameObject, SaveSlotViewModel->SaveSlotName, 0);
}
