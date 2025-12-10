// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AuraGameInstance.generated.h"

class UCharacterClassInfo;
class UAbilityInfo;

UCLASS()
class AURA_API UAuraGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "System|Audio")
	void SetGlobalMusicVolume(float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "System|Audio")
	float GetGlobalMusicVolume() const { return GlobalMusicVolume; }

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "System|Audio")
	float GlobalMusicVolume = 0.5f;
};
