// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "Character/BlasterCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerController/BlasterPlayerControllerBase.h"
#include "PlayerState/BlasterPlayerState.h"

ABlasterGameMode::ABlasterGameMode()
{
	bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
	Super::BeginPlay();
	LevelStartingTime = GetWorld()->GetTimeSeconds();
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
}

void ABlasterGameMode::PlayerEliminated(class ABlasterCharacterBase* ElimmedCharacter,
                                        class ABlasterPlayerControllerBase* VictimController, class ABlasterPlayerControllerBase* AttackerController)
{
	ABlasterPlayerState* AttackerPlayerState = AttackerController ? AttackerController->GetPlayerState<ABlasterPlayerState>() : nullptr;
	ABlasterPlayerState* VictimPlayerState = VictimController ? VictimController->GetPlayerState<ABlasterPlayerState>() : nullptr;

	if (AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);	
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

void ABlasterGameMode::OnMatchStateSet()
{
    Super::OnMatchStateSet();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        ABlasterPlayerControllerBase* BlasterPlayer = Cast<ABlasterPlayerControllerBase>(*It);
        if (BlasterPlayer)
        {
            BlasterPlayer->OnMatchStateSet(MatchState);
        }
    }
}