// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LoadMenuHUD.generated.h"

class ULoadMenuWidget;
class UMVVM_LoadMenu;

UCLASS()
class AURA_API ALoadMenuHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<ULoadMenuWidget> LoadMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<ULoadMenuWidget> LoadMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMVVM_LoadMenu> LoadMenuViewModelClass;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	TObjectPtr<UMVVM_LoadMenu> LoadMenuViewModel;
protected:
	virtual void BeginPlay() override;
};
