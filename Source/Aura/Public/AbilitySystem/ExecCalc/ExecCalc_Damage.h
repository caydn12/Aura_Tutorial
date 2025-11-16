// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

class UAbilitySystemComponent;
class UCharacterClassInfo;

UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
public:
	UExecCalc_Damage();
protected:
public:
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
	void DetermineDebuff(const FGameplayEffectSpec& Spec, const FGameplayEffectCustomExecutionParameters& ExecutionParams, FAggregatorEvaluateParameters& EvaluationParameters) const;
	float ApplyDamageReductionByHaloOfProtection(float Damage, const int32 TargetLevel, const UAbilitySystemComponent* TargetASC, const UCharacterClassInfo* TargetCharacterClassInfo) const;
};
