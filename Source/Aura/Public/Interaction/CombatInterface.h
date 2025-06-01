// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

// This class does not need to be modified.
// BlueprintType was added to expose the combat interface for use in blueprints
UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

class UAnimMontage;

class AURA_API ICombatInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual int32 GetPlayerLevel() const;
	virtual void Die() = 0;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation();

	// Blueprint Implementable Events cannot be virtual.
	// Blueprint Implementable Events do not have C++ Definitions
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	// Blueprint Native Events are virtual.
	// Automatically generates a virtual native version in C++ that can be overriden in C++.
	// _Implementation in cpp declaration to override.
	// "Both Worlds" use case for implementing functions in blueprint and C++
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();
};
