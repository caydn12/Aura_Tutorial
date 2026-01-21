// Copyright Kickback Studio


#include "Game/LoadMenuSaveGame.h"

FSavedMap ULoadMenuSaveGame::GetSavedMapWithMapName(const FString& InMapName) const
{
    FSavedMap FoundMap = FSavedMap();
    for (const FSavedMap& Map : SavedMaps)
    {
        if (Map.MapAssetName == InMapName)
        {
            FoundMap = Map;
			break;
        }
    }
	return FoundMap;
}

bool ULoadMenuSaveGame::DoesSavedMapExist(const FString& InMapName) const
{
    bool bFoundMap = false;
    for (const FSavedMap& Map : SavedMaps)
    {
        if (Map.MapAssetName == InMapName)
        {
            bFoundMap = true;
            break;
        }
    }
	return bFoundMap;
}

void ULoadMenuSaveGame::UpdateLevelClearedMap(TMap<ELevelID, bool> InNewLevelClearedMap)
{
    LevelClearedMap.Empty();
	LevelClearedMap = InNewLevelClearedMap;
}
