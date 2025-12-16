// Copyright Kickback Studio


#include "UI/ViewModel/MVVM_LoadMenuSaveSlot.h"

void UMVVM_LoadMenuSaveSlot::SetSaveSlotWidget(const ESaveSlotWidget SaveSlotWidget)
{
	const int32 WidgetSwitcherIndex = static_cast<int32>(SaveSlotWidget);
	OnSetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadMenuSaveSlot::SetPlayerName(FString InPlayerName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayerName);
}

void UMVVM_LoadMenuSaveSlot::SetSaveSlotName(FString InSaveSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SaveSlotName, InSaveSlotName);
}
