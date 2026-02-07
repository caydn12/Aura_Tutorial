// Copyright Kickback Studio


#include "AbilitySystem/Data/LootTiers.h"

TArray<FLootItem> ULootTiers::GetLootItems() const
{
    TArray<FLootItem> Items;

    for (FLootItem Item : LootItems)
    {
        for (int32 i = 0; i < Item.MaxSpawns; ++i)
        {
            if (FMath::FRandRange(1.f, 100.f) < Item.SpawnChance)
            {
                FLootItem NewItem;
				NewItem.LootClass = Item.LootClass;
                NewItem.bLootLevelOverride = Item.bLootLevelOverride;

                Items.Add(NewItem);
			}
        }
    }

    return Items;
}
