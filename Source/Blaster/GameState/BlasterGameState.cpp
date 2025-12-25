
#include "BlasterGameState.h"
#include "GameMode/BlasterBaseGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/BlasterPlayerState.h"

ABlasterGameState::ABlasterGameState()
{
	bReplicates = true;
	bIsVotingActive = false;
	VotingTimeRemaining = 0.f;
	WinningMapIndex = INDEX_NONE;
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0)
	{
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlasterGameState::SetVotingState(bool bNewActive, float Duration,
                                       const TArray<FMapVoteOption>& NewMapOptions,
                                       const TArray<int32>& NewVoteCounts,
                                       int32 NewWinningIndex)
{
	bIsVotingActive = bNewActive;
	VotingTimeRemaining = Duration;
	MapOptions = NewMapOptions;
	VoteCounts = NewVoteCounts;
	WinningMapIndex = NewWinningIndex;
}

void ABlasterGameState::UpdateVoteCounts(const TArray<int32>& NewVoteCounts)
{
	VoteCounts = NewVoteCounts;
	OnRep_VoteCounts();
}

void ABlasterGameState::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, MapOptions);
	DOREPLIFETIME(ABlasterGameState, VoteCounts);
	DOREPLIFETIME(ABlasterGameState, VotingTimeRemaining);
	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
}

void ABlasterGameState::OnRep_VoteCounts()
{
	OnVoteCountsChanged.Broadcast(VoteCounts);
}