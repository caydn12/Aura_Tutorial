// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenu.h"
#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"

void UMVVM_LoadMenu::InitializeSaveSlots()
{
	SaveSlot_0 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
	SaveSlotViewModels.Add(0, SaveSlot_0);

	SaveSlot_1 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
	SaveSlotViewModels.Add(1, SaveSlot_1);

	SaveSlot_2 = NewObject<UMVVM_LoadMenuSaveSlot>(this, LoadMenuSaveSlotViewModelClass);
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
	
}

void UMVVM_LoadMenu::SelectSaveSlotButtonPressed(int32 SlotIndex)
{

}
