// Copyright Kickback Studio


#include "Input/AuraInputConfig.h"
#include "InputAction.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLog) const
{
	const UInputAction* FoundAction = nullptr;
	for (const FAuraInputAction& Action : AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag == InputTag)
		{
			FoundAction = Action.InputAction;
		}
	}

	if (!FoundAction && bLog)
	{
		UE_LOG(LogTemp, Error,
			TEXT("Can't find AbilityInputAction for Input Tag [%s], on Input Config [%s]"), 
				*InputTag.ToString(),
				*GetNameSafe(this));
	}

	return FoundAction;
}
