// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"

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
	SaveSlotViewModels[SlotIndex]->OnSetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadMenu::NewSaveSlotButtonPressed(int32 SlotIndex, const FString& EnteredName)
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(this)))
	{
		SaveSlotViewModels[SlotIndex]->SetPlayerName(EnteredName);

		AuraGameInstance->SaveSlotData(SaveSlotViewModels[SlotIndex]);

		// Temporary UI Force change
		SaveSlotViewModels[SlotIndex]->InitializeSlot();
	}
}

void UMVVM_LoadMenu::SelectSaveSlotButtonPressed(int32 SlotIndex)
{

}
