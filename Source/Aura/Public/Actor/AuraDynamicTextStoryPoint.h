// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraStoryPoint.h"
#include "Interaction/SaveInterface.h"
#include "AuraDynamicTextStoryPoint.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraDynamicTextStoryPoint : public AAuraStoryPoint, public ISaveInterface
{
	GENERATED_BODY()

public:

	// Save Interface
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	// End Save Interface

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bGameWon = false;

	UFUNCTION(BlueprintCallable)
	void SetStoryPointText(const FText& NewText);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "StoryPoint|Dynamic")
	FText CompletedText = FText::FromString(TEXT("You won the game!"));

private:
	UFUNCTION()
	void HandleCompletedGoals();

	UPROPERTY()
	TObjectPtr<class UAuraGameInstance> CachedGameInstance;
};
