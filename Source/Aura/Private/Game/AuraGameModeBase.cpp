// Copyright Kickback Studio


#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "GameFramework/Character.h"
#include "Game/AuraGameInstance.h"
#include "Game/LoadMenuSaveGame.h"

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance());

	APlayerStart* ChosenStart = nullptr;

	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (PlayerStartActors.Num() > 0)
	{
		for (AActor* AvailablePlayerStartActor : PlayerStartActors)
		{
			if (APlayerStart* AvailablePlayerStart = Cast<APlayerStart>(AvailablePlayerStartActor))
			{
				if (AvailablePlayerStart->PlayerStartTag == AuraGameInstance->PlayerStartTag)
				{
					ChosenStart = AvailablePlayerStart;
					break;
				}
			}
		}

		if (ChosenStart == nullptr)
		{
			ChosenStart = Cast<APlayerStart>(PlayerStartActors[0]);
		}
	}


	return ChosenStart;
}

void AAuraGameModeBase::PlayerDied(ACharacter* DeadCharacter)
{
	if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance()))
	{
		ULoadMenuSaveGame* SaveGame = AuraGameInstance->GetInGameSaveData();
		if (SaveGame)
		{
			UGameplayStatics::OpenLevel(DeadCharacter, FName(SaveGame->MapAssetName));
		}
	}
}
