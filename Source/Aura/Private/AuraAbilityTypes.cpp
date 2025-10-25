#include "AuraAbilityTypes.h"

TArray<FVector_NetQuantize> FAuraGameplayEffectContext::GetSpawnLocations() const
{
	return SpawnLocations;
}

void FAuraGameplayEffectContext::SetSpawnLocations(const TArray<FVector_NetQuantize> InSpawnLocations)
{
	SpawnLocations.Reset();
	SpawnLocations.Append(InSpawnLocations);
}

bool FAuraGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	Super::NetSerialize(Ar, Map, bOutSuccess);

	enum RepFlag
	{
		REP_IsBlockedHit = 0,
		REP_IsCriticalHit = 1,
		REP_IsSuccessfulDebuff = 2,
		REP_DebuffDamage = 3,
		REP_DebuffDuration = 4,
		REP_DebuffFrequency = 5,
		REP_DamageType = 6,
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
		if (bIsSuccessfulDebuff)
		{
			RepBits |= (1 << REP_IsSuccessfulDebuff);
		}
		if (DebuffDamage > 0.f)
		{
			RepBits |= (1 << REP_DebuffDamage);
		}
		if (DebuffDuration > 0.f)
		{
			RepBits |= (1 << REP_DebuffDuration);
		}
		if (DebuffFrequency > 0.f)
		{
			RepBits |= (1 << REP_DebuffFrequency);
		}
		if (DamageType.IsValid())
		{
			RepBits |= (1 << REP_DamageType);
		}
	}

	Ar.SerializeBits(&RepBits, REP_MAX);

	if (RepBits & (1 << REP_DebuffDamage))
	{
		Ar << DebuffDamage;
	}

	if (RepBits & (1 << REP_DebuffDuration))
	{
		Ar << DebuffDuration;
	}

	if (RepBits & (1 << REP_DebuffFrequency))
	{
		Ar << DebuffFrequency;
	}

	if (RepBits & (1 << REP_DamageType))
	{
		if (Ar.IsLoading())
		{
			if (!DamageType.IsValid())
			{
				DamageType = TSharedPtr<FGameplayTag>(new FGameplayTag());
			}
		}
		DamageType->NetSerialize(Ar, Map, bOutSuccess);
	}

	if (Ar.IsLoading())
	{
		bIsBlockedHit = RepBits & (1 << REP_IsBlockedHit);
		bIsCriticalHit = RepBits & (1 << REP_IsCriticalHit);
		bIsSuccessfulDebuff = RepBits & (1 << REP_IsSuccessfulDebuff);
	}

	SafeNetSerializeTArray_WithNetSerialize<31, FVector_NetQuantize>(Ar, SpawnLocations, Map);

	bOutSuccess = true;
	return true;
}