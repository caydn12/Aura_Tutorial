// Copyright Kickback Studio


#include "Actor/AuraStoryPoint.h"
#include "Components/WidgetComponent.h"

AAuraStoryPoint::AAuraStoryPoint()
{
	Root = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(Root);

	StoryPointMesh = CreateDefaultSubobject<USkeletalMeshComponent>("StoryPointMesh");
	StoryPointMesh->SetupAttachment(Root);

	StoryText = CreateDefaultSubobject<UWidgetComponent>("StoryText");
	StoryText->SetupAttachment(StoryPointMesh);
}

void AAuraStoryPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

