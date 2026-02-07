// Copyright Kickback Studio


#include "Actor/AuraDynamicTextStoryPoint.h"
#include "Components/WidgetComponent.h"
#include "UI/Widget/AuraStoryTextWidget.h"
#include "Game/AuraGameInstance.h"

void AAuraDynamicTextStoryPoint::LoadActor_Implementation()
{
	// Not necessary in this actor. Must be implemented.
}

void AAuraDynamicTextStoryPoint::SetStoryPointText(const FText& NewText)
{
	if (StoryText)
	{
		if (UAuraStoryTextWidget* StoryWidget = Cast<UAuraStoryTextWidget>(StoryText->GetUserWidgetObject()))
		{
			StoryWidget->SetStoryText(NewText);
		}
	}
}

void AAuraDynamicTextStoryPoint::BeginPlay()
{
	Super::BeginPlay();

	CachedGameInstance = GetGameInstance<UAuraGameInstance>();
	if (CachedGameInstance)
	{
		if (CachedGameInstance->AreAllGoalpointsCompleted())
		{
			HandleCompletedGoals();
		}
		else
		{
			CachedGameInstance->OnGoalpointsCompletedDelegate.AddUObject(this, &AAuraDynamicTextStoryPoint::HandleCompletedGoals);
		}
	}
}

void AAuraDynamicTextStoryPoint::HandleCompletedGoals()
{
	SetStoryPointText(CompletedText);
	bGameWon = true;
}
