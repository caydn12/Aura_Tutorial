// Copyright Kickback Studio

#include "AuraSystemLibrary.h"
#include "GameFramework/GameUserSettings.h"

void UAuraSystemLibrary::AdjustScalability(EScalabilitySetting ScalabilitySetting)
{
	const int32 CurrentQualityLevel = UGameUserSettings::GetGameUserSettings()->GetOverallScalabilityLevel();
	if (CurrentQualityLevel != (int32)ScalabilitySetting)
	{
		UGameUserSettings::GetGameUserSettings()->SetOverallScalabilityLevel((int32)ScalabilitySetting);
		UGameUserSettings::GetGameUserSettings()->ApplySettings(false);
	}
}