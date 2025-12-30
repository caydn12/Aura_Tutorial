// Copyright Kickback Studio


#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"

AActor* AAuraGameModeBase::ChoosePlayerStart_Implementation(AController* Player)
{
	APlayerStart* ChosenStart = nullptr;

	TArray<AActor*> PlayerStartActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStartActors);

	if (PlayerStartActors.Num() > 0)
	{
		for (AActor* AvailablePlayerStartActor : PlayerStartActors)
		{
			if (APlayerStart* AvailablePlayerStart = Cast<APlayerStart>(AvailablePlayerStartActor))
			{
				if (AvailablePlayerStart->PlayerStartTag == FName("TheTag"))
				{
					ChosenStart = AvailablePlayerStart;
					break;
				}
			}
		}
	}

	return ChosenStart;
}
