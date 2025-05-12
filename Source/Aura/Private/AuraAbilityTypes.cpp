#include "AuraAbilityTypes.h"

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	enum RepFlag
	{
		REP_IsBlockedHit = 0,
		REP_IsCriticalHit = 1,
		REP_MAX
	};

	uint32 RepBits = 0;
	if (Ar.IsSaving())
	{
		if (bIsBlockedHit)
		{
			RepBits |= (1 << REP_IsBlockedHit);
		}
		if (bIsCriticalHit)
		{
			RepBits |= (1 << REP_IsCriticalHit);
		}
	}

	Ar.SerializeBits(&RepBits, REP_MAX);

	if (Ar.IsLoading())
	{
		bIsBlockedHit = RepBits & (1 << REP_IsBlockedHit);
		bIsCriticalHit = RepBits & (1 << REP_IsCriticalHit);
	}

	bOutSuccess = true;
	return true;
}