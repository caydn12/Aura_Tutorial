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

void UMVVM_LoadMenuSaveSlot::SetMapName(FString InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadMenuSaveSlot::SetPlayerStartTag(FName InPlayerStartTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerStartTag, InPlayerStartTag);
	
	FString StartTagTrimmed = PlayerStartTag.ToString().RightChop(8);
	StartTagTrimmed.InsertAt(StartTagTrimmed.Len() - 1, TEXT(" "));
	SetPlayerStartTagTrimmed(StartTagTrimmed);
}

void UMVVM_LoadMenuSaveSlot::SetPlayerStartTagTrimmed(FString InPlayerStartTagTrimmed)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerStartTagTrimmed, InPlayerStartTagTrimmed);
}

void UMVVM_LoadMenuSaveSlot::SetPlayerLevel(int32 InLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InLevel);
}
