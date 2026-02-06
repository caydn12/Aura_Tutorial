// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraStoryTextWidget.generated.h"


UCLASS()
class AURA_API UAuraStoryTextWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetStoryText(const FText& StoryText);
};
