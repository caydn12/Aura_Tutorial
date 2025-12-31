// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadMenuSaveSlot.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitcherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);

UENUM(BlueprintType)
enum class ESaveSlotWidget : uint8
{
	ESS_Vacant,
	ESS_EnterName,
	ESS_Taken
};

UCLASS()
class AURA_API UMVVM_LoadMenuSaveSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:

	void SetSaveSlotWidget(const ESaveSlotWidget SaveSlotWidget);

	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex OnSetWidgetSwitcherIndex;

	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton OnEnableSelectSlotButton;

	// Field Notifies for UI Binding

	void SetPlayerName(FString InPlayerName);
	void SetSaveSlotName(FString InSaveSlotName);
	void SetMapName(FString InMapName);
	void SetPlayerStartTag(FName InPlayerStartTag);

	FString GetPlayerName() const { return PlayerName; }
	FString GetSaveSlotName() const { return SaveSlotName; }
	FString GetMapName() const { return MapName; }
	FName GetPlayerStartTag() const { return PlayerStartTag; }

private:
	// Field Notifies for UI Binding

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"))
	FString SaveSlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess="true"))
	FString PlayerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"))
	FString MapName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, FieldNotify, Setter, Getter, meta = (AllowPrivateAccess = "true"))
	FName PlayerStartTag;
};
