
#include "BlasterBaseGameMode.h"
#include "GameState/BlasterGameState.h"
#include "PlayerState/BlasterPlayerState.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "PlayerController/BlasterPlayerControllerBase.h"

ABlasterBaseGameMode::ABlasterBaseGameMode()
{
    GameStateClass = ABlasterGameState::StaticClass();
    SessionDuration = 20.f;
    bIsVotingActive = false;
    WinningMapIndex = INDEX_NONE;
}

ABlasterGameState* ABlasterBaseGameMode::GetBlasterGameState() const
{
    return GetGameState<ABlasterGameState>();
}

void ABlasterBaseGameMode::StartMapVote()
{
    if (bIsVotingActive || MapOptions.Num() == 0)
    {
        return;
    }
    
    bIsVotingActive = true;
    WinningMapIndex = INDEX_NONE;
    
    VoteCounts.SetNum(MapOptions.Num());
    for (int32 i = 0; i < VoteCounts.Num(); ++i)
    {
        VoteCounts[i] = 0;
    }

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        if (APlayerController* PC = It->Get())
        {
            if (ABlasterPlayerState* PS = PC->GetPlayerState<ABlasterPlayerState>())
            {
                PS->ChosenMapIndex = INDEX_NONE;
                PS->bHasVoted = false;
            }
        }
    }
    
    if (ABlasterGameState* GS = GetBlasterGameState())
    {
        GS->SetVotingState(true, SessionDuration, MapOptions, VoteCounts, INDEX_NONE);
    }

    GetWorldTimerManager().SetTimer(
        VotingTimerHandle,
        this,
        &ABlasterBaseGameMode::EndMapVote,
        SessionDuration,
        false
    );
}

void ABlasterBaseGameMode::EndMapVote()
{
    if (!bIsVotingActive)
    {
        return;
    }
    
    bIsVotingActive = false;
    GetWorldTimerManager().ClearTimer(VotingTimerHandle);
    
    int32 BestIndex = INDEX_NONE;
    int32 BestVotes = -1;

    for (int32 i = 0; i < VoteCounts.Num(); ++i)
    {
        if (VoteCounts[i] > BestVotes)
        {
            BestVotes = VoteCounts[i];
            BestIndex = i;
        }
    }

    WinningMapIndex = BestIndex;

    if (ABlasterGameState* GS = GetBlasterGameState())
    {
        GS->SetVotingState(false, 0.f, MapOptions, VoteCounts, WinningMapIndex);
    }

    OnVoteCompleted(WinningMapIndex);
}

void ABlasterBaseGameMode::RegisterVote(int32 MapIndex, ABlasterPlayerState* VotingPlayerState)
{
    if (!bIsVotingActive || !VotingPlayerState)
    {
        return;
    }

    if (!MapOptions.IsValidIndex(MapIndex))
    {
        return;
    }

    if (VotingPlayerState->bHasVoted)
    {
        int32 OldIndex = VotingPlayerState->ChosenMapIndex;
        if (VoteCounts.IsValidIndex(OldIndex))
        {
            VoteCounts[OldIndex] = FMath::Max(0, VoteCounts[OldIndex] - 1);
        }
    }

    VotingPlayerState->ChosenMapIndex = MapIndex;
    VotingPlayerState->bHasVoted = true;
    VotingPlayerState->ClientSetLobbyWidgetVoteTick();
    VoteCounts[MapIndex]++;

    if (ABlasterGameState* GS = GetBlasterGameState())
    {
        GS->UpdateVoteCounts(VoteCounts);
    }
}

void ABlasterBaseGameMode::OnVoteCompleted(int32 ChosenMapIndex)
{
}