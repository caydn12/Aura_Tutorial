// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadMenu.generated.h"

class UMVVM_LoadMenuSaveSlot;

UCLASS()
class AURA_API UMVVM_LoadMenu : public UMVVMViewModelBase
{
	GENERATED_BODY()
	
public:
	void InitializeSaveSlots();

	UFUNCTION(BlueprintPure)
	UMVVM_LoadMenuSaveSlot* GetSaveSlotViewModelByIndex(int32 Index) const;

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 SlotIndex);

	UFUNCTION(BlueprintCallable)
	void NewSaveSlotButtonPressed(int32 SlotIndex, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void SelectSaveSlotButtonPressed(int32 SlotIndex);

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadMenuSaveSlot> LoadMenuSaveSlotViewModelClass;

private:
	UPROPERTY()
	TMap<int32, UMVVM_LoadMenuSaveSlot*> SaveSlotViewModels;

	// Prevent garbage collection

	UPROPERTY()
	TObjectPtr<UMVVM_LoadMenuSaveSlot> SaveSlot_0;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadMenuSaveSlot> SaveSlot_1;

	UPROPERTY()
	TObjectPtr<UMVVM_LoadMenuSaveSlot> SaveSlot_2;
};
