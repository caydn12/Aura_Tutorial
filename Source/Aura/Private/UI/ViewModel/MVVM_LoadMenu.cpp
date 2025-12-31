// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"
#include "Game/AuraGameModeBase.h"

void UMVVM_LoadMenu::InitializeSaveSlots()
{
	SaveSlot_0 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
	SaveSlot_0->SetSaveSlotName(FString("SaveSlot_0"));
	SaveSlotViewModels.Add(0, SaveSlot_0);

	SaveSlot_1 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
	SaveSlot_1->SetSaveSlotName(FString("SaveSlot_1"));
	SaveSlotViewModels.Add(1, SaveSlot_1);

	SaveSlot_2 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
	SaveSlot_2->SetSaveSlotName(FString("SaveSlot_2"));
	SaveSlotViewModels.Add(2, SaveSlot_2);

	SetNumSaveSlots(SaveSlotViewModels.Num());
}

UMVVM_LoadMenuSaveSlot* UMVVM_LoadMenu::GetSaveSlotViewModelByIndex(int32 Index) const
{
	return SaveSlotViewModels.FindChecked(Index);
}

void UMVVM_LoadMenu::NewGameButtonPressed(int32 SlotIndex)
{
	SaveSlotViewModels[SlotIndex]->SetSaveSlotWidget(ESaveSlotWidget::ESS_EnterName);
}

void UMVVM_LoadMenu::NewSaveSlotButtonPressed(int32 SlotIndex, const FString& EnteredName)
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SaveSlotViewModels[SlotIndex]->SetPlayerName(EnteredName);
		SaveSlotViewModels[SlotIndex]->SetMapName(AuraGameInstance->DefaultMapName);

		if (AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(this)))
		{
			SaveSlotViewModels[SlotIndex]->SetPlayerStartTag(AuraGameMode->DefaultPlayerStartTag);
		}

		AuraGameInstance->SaveSlotData(SaveSlotViewModels[SlotIndex]);

		SaveSlotViewModels[SlotIndex]->SetSaveSlotWidget(ESaveSlotWidget::ESS_Taken);
	}
}

void UMVVM_LoadMenu::SelectSaveSlotButtonPressed(int32 SlotIndex)
{
	OnSlotSelected.Broadcast();
	SelectedSaveSlotViewModel = SaveSlotViewModels[SlotIndex];
	for (const TTuple<int32, UMVVM_LoadMenuSaveSlot*> SaveSlotViewModel : SaveSlotViewModels)
	{
		// Enable the select button for all slots except the one that was just selected
		bool bEnable = SaveSlotViewModel.Key != SlotIndex;
		SaveSlotViewModel.Value->OnEnableSelectSlotButton.Broadcast(bEnable);
	}
}

void UMVVM_LoadMenu::DeleteButtonPressed()
{
	if (IsValid(SelectedSaveSlotViewModel))
	{
		UAuraGameInstance::DeleteSaveSlot(SelectedSaveSlotViewModel->GetSaveSlotName());
		SelectedSaveSlotViewModel->SetSaveSlotWidget(ESaveSlotWidget::ESS_Vacant);
		SelectedSaveSlotViewModel->OnEnableSelectSlotButton.Broadcast(true);
	}
}

void UMVVM_LoadMenu::PlayButtonPressed()
{
	// Game instance can also be found via AuraGameMode->GetGameInstance()
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		if (IsValid(SelectedSaveSlotViewModel))
		{
			AuraGameInstance->PlayerStartTag = SelectedSaveSlotViewModel->GetPlayerStartTag();
			AuraGameInstance->SaveSlotName = SelectedSaveSlotViewModel->GetSaveSlotName();

			AuraGameInstance->TravelToMap(SelectedSaveSlotViewModel);
		}
	}
}

void UMVVM_LoadMenu::LoadSaveData()
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		for (const TTuple<int32, UMVVM_LoadMenuSaveSlot*> SaveSlotViewModel : SaveSlotViewModels)
		{
			ULoadMenuSaveGame* LoadMenuSaveObject = AuraGameInstance->GetSaveSlotData(SaveSlotViewModel.Value->GetSaveSlotName());

			if (LoadMenuSaveObject)
			{
				SaveSlotViewModel.Value->SetPlayerName(LoadMenuSaveObject->PlayerName);
				// LoadMenuSaveGame.h defaults PlayerName to "Default Name".
				// Save data is always created, and will have default data if unaffected by the user.
				// In that case, we want to show the slot as vacant. If the name has changed, the slot is taken by the user.
				if (LoadMenuSaveObject->PlayerName == "Default Name") 
				{
					SaveSlotViewModel.Value->SetSaveSlotWidget(ESaveSlotWidget::ESS_Vacant);
				}
				else
				{
					SaveSlotViewModel.Value->SetSaveSlotWidget(ESaveSlotWidget::ESS_Taken);
					SaveSlotViewModel.Value->SetMapName(LoadMenuSaveObject->MapName);
					SaveSlotViewModel.Value->SetPlayerStartTag(LoadMenuSaveObject->PlayerStartTag);
				}
			}
		}
	}
}

void UMVVM_LoadMenu::SetNumSaveSlots(int32 InNumSaveSlots)
{
	UE_MVVM_SET_PROPERTY_VALUE(NumSaveSlots, InNumSaveSlots);
}
