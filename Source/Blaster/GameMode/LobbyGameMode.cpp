// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();

	if (NumberOfPlayers >= 2)
	{
		StartMapVote();
	}
}

void ALobbyGameMode::OnVoteCompleted(int32 ChosenMapIndex)
{
	Super::OnVoteCompleted(ChosenMapIndex);

	if (!MapOptions.IsValidIndex(ChosenMapIndex))
	{
		return;
	}
	
	const FMapVoteOption& Option = MapOptions[ChosenMapIndex];
	FString TravelURL = Option.MapPath.ToString() + TEXT("?listen");
	GetWorld()->ServerTravel(TravelURL);
}