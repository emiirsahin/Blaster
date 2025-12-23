
#include "BlasterGameState.h"
#include "GameMode/BlasterBaseGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerController/BlasterPlayerControllerBase.h"

ABlasterGameState::ABlasterGameState()
{
	bReplicates = true;
	bIsVotingActive = false;
	VotingTimeRemaining = 0.f;
	WinningMapIndex = INDEX_NONE;
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
	OnRep_IsVotingActive();
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
	DOREPLIFETIME(ABlasterGameState, bIsVotingActive);
}

void ABlasterGameState::OnRep_VoteCounts()
{
	OnVoteCountsChanged.Broadcast(VoteCounts);
}

void ABlasterGameState::OnRep_IsVotingActive()
{
	UWorld* World = GetWorld();
	if (World)
	{
		ABlasterPlayerControllerBase* BPC =  World->GetFirstPlayerController<ABlasterPlayerControllerBase>();
		if (BPC)
		{
			BPC->ToggleLobbyWidget(bIsVotingActive);
			BPC->SetIsVotingActive(bIsVotingActive);
		}
	}
}