// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraSystemLibrary.generated.h"

UENUM(BlueprintType)
enum class EScalabilitySetting : uint8
{
	Low = 0,
	Medium,
	High,
	Epic
};

UCLASS()
class AURA_API UAuraSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable, Category = "AuraSystemLibrary|System")
	static void AdjustScalability(EScalabilitySetting ScalabilitySetting);

	UFUNCTION(BlueprintCallable, Category = "AuraSystemLibrary|System")
	static void SetGlobalMusicVolume(const UObject* WorldContextObject, float NewVolume);

	UFUNCTION(BlueprintCallable, Category = "AuraSystemLibrary|System")
	static float GetGlobalMusicVolume(const UObject* WorldContextObject);
};
