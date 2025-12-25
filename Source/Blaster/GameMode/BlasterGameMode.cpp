// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Character/BlasterCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerController/BlasterPlayerControllerBase.h"
#include "PlayerState/BlasterPlayerState.h"
#include "GameState/BlasterGameState.h"

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
	SetMatchState(MatchState::WaitingToStart);
}

void ABlasterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (MatchState == MatchState::WaitingToStart)
	{
		CountdownTime = WarmupTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime < 0.f)
		{
			StartMatch();
		}
	}
	else if (MatchState == MatchState::InProgress)
	{
		CountdownTime = WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime < 0.f)
		{
			SetMatchState(MatchState::Cooldown);
		}
	}
	else if (MatchState == MatchState::Cooldown)
	{
		CountdownTime = CooldownTime + WarmupTime + MatchTime - GetWorld()->GetTimeSeconds() + LevelStartingTime;

		if (CountdownTime < 0.f)
		{
			StartVotingProcess();
		}
	}
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacterBase* ElimmedCharacter,
                                        class ABlasterPlayerControllerBase* VictimController, class ABlasterPlayerControllerBase* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? AttackerController->GetPlayerState<ABlasterPlayerState>() : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? VictimController->GetPlayerState<ABlasterPlayerState>() : nullptr;

	ABlasterGameState* BlasterGameState = GetGameState<ABlasterGameState>();
		
	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState && BlasterGameState)
	{
		AttackerPlayerState->AddToScore(1.f);
		BlasterGameState->UpdateTopScore(AttackerPlayerState);
	}
	if (VictimPlayerState)
	{
		VictimPlayerState->AddToDefeats(1);
		if (AttackerPlayerState)
		{
			VictimController->ClientAddToElimText(AttackerPlayerState->GetPlayerName());
		}
	}
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
	}
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if (ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	TArray<AActor*> PlayerStarts;
	if (ElimmedController)
	{
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), PlayerStarts);
		int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

void ABlasterGameMode::OnVoteCompleted(int32 ChosenMapIndex)
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