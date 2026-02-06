// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraStoryPoint.generated.h"

class UWidgetComponent;

UCLASS()
class AURA_API AAuraStoryPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraStoryPoint();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USkeletalMeshComponent> StoryPointMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> StoryText;

	UPROPERTY(EditAnywhere)
	FText StoryPointText;

public:	

};
