// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	AAuraPlayerController();

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

public:
	virtual void PlayerTick(float DeltaTime) override;

private:
	UAuraAbilitySystemComponent* GetASC();

	void Move(const FInputActionValue& InputActionValue);
	void CursorTrace();
	void AutoRun();

	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;

	// AuraInputComponent BindAbilityActions Callback Functions
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	TScriptInterface<IEnemyInterface> LastHoveredActor;
	TScriptInterface<IEnemyInterface> HoveredActor;
	FHitResult CursorHit;

	// Click Movement
	FVector CachedDestination = FVector::ZeroVector;
	float FollowTime = 0.0f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.0f;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
};
