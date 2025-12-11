// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadMenuSaveSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);

UCLASS()
class AURA_API UMVVM_LoadMenuSaveSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:

	void InitializeSlot();

	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex OnSetWidgetSwitcherIndex;

	UPROPERTY()
	FString PlayerName;

	UPROPERTY()
	FString SaveSlotName;
};
