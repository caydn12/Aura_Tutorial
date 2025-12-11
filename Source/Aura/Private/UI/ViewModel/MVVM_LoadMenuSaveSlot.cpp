// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"

void UMVVM_LoadMenuSaveSlot::InitializeSlot()
{
	OnSetWidgetSwitcherIndex.Broadcast(2);
}

void UMVVM_LoadMenuSaveSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadMenuSaveSlot::SetSaveSlotName(FString InSaveSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SaveSlotName, InSaveSlotName);
}
