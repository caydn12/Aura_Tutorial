// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "Game/LoadMenuSaveGame.h"

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

		AuraGameInstance->SaveSlotData(SaveSlotViewModels[SlotIndex]);

		SaveSlotViewModels[SlotIndex]->SetSaveSlotWidget(ESaveSlotWidget::ESS_Taken);
	}
}

void UMVVM_LoadMenu::SelectSaveSlotButtonPressed(int32 SlotIndex)
{

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
				if (LoadMenuSaveObject->PlayerName.IsEmpty()) // LoadMenuSaveGame.h defaults PlayerName to ""
				{
					SaveSlotViewModel.Value->SetSaveSlotWidget(ESaveSlotWidget::ESS_Vacant);
				}
				else
				{
					SaveSlotViewModel.Value->SetSaveSlotWidget(ESaveSlotWidget::ESS_Taken);
				}
			}
		}
	}
}
