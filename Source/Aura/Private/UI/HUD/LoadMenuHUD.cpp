// Copyright Kickback Studio


#include "UI/HUD/LoadMenuHUD.h"
#include "UI/Widget/LoadMenuWidget.h"
#include "UI/ViewModel/MVVM_LoadMenu.h"

void ALoadMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	LoadMenuViewModel = NewObject<UMVVM_LoadMenu>(this, LoadMenuViewModelClass);
	LoadMenuViewModel->InitializeSaveSlots();

	LoadMenuWidget = CreateWidget<ULoadMenuWidget>(GetWorld(), LoadMenuWidgetClass);
	LoadMenuWidget->AddToViewport();
	LoadMenuWidget->BlueprintInitializeWidget();
}
