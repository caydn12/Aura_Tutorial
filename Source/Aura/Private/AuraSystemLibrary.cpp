// Copyright Kickback Studio

#include "AuraSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"
#include "Game/AuraGameInstance.h"
#include "Kismet/GameplayStatics.h"

void UAuraSystemLibrary::AdjustScalability(EScalabilitySetting ScalabilitySetting)
{
	const int32 CurrentQualityLevel = UGameUserSettings::GetGameUserSettings()->GetOverallScalabilityLevel();
	if (CurrentQualityLevel != (int32)ScalabilitySetting)
	{
		UGameUserSettings::GetGameUserSettings()->SetOverallScalabilityLevel((int32)ScalabilitySetting);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}
}

void UAuraSystemLibrary::SetGlobalMusicVolume(const UObject* WorldContextObject, float NewVolume)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (AuraGameInstance)
	{
		AuraGameInstance->SetGlobalMusicVolume(NewVolume);
	}
}

float UAuraSystemLibrary::GetGlobalMusicVolume(const UObject* WorldContextObject)
{
	float Volume = 0.0f;
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	if (AuraGameInstance)
	{
		Volume = AuraGameInstance->GetGlobalMusicVolume();
	}
	return Volume;
}
