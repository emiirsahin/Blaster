// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyWidget.h"
#include "Components/Button.h"
#include "Components/ProgressBar.h"
#include "GameState/BlasterGameState.h"
#include "PlayerController/BlasterPlayerControllerBase.h"

void ULobbyWidget::LobbyDisplaySetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Collapsed);
	SetIsFocusable(false);
}

void ULobbyWidget::ToggleLobbyWidget(bool bShow)
{
	if (!bShow)
	{
		TogglePlayerControllerWidgetInteraction(false);
		SetVisibility(ESlateVisibility::Collapsed);
		SetIsFocusable(false);
	}
	else
	{
		TogglePlayerControllerWidgetInteraction(true);
		SetVisibility(ESlateVisibility::Visible);
		SetIsFocusable(true);
	}
}

void ULobbyWidget::UpdateProgressBars(int32 LeftMapVotes, int32 RightMapVotes)
{
	const int32 TotalVotes = LeftMapVotes + RightMapVotes;

	if (TotalVotes <= 0)
	{
		LeftMapProgressBar->SetPercent(0.f);
		RightMapProgressBar->SetPercent(0.f);
		return;
	}

	const float LeftPercent  = static_cast<float>(LeftMapVotes)  / static_cast<float>(TotalVotes);
	const float RightPercent = static_cast<float>(RightMapVotes) / static_cast<float>(TotalVotes);

	LeftMapProgressBar->SetPercent(LeftPercent);
	RightMapProgressBar->SetPercent(RightPercent);

	LeftMapProgressBarText->SetText(FText::AsNumber(LeftMapVotes));
	RightMapProgressBarText->SetText(FText::AsNumber(RightMapVotes));
}

bool ULobbyWidget::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (LeftMapButton)
	{
		LeftMapButton->OnClicked.AddDynamic(this, &ThisClass::LeftMapButtonClicked);
	}

	if (RightMapButton)
	{
		RightMapButton->OnClicked.AddDynamic(this, &ThisClass::RightMapButtonClicked);
	}
	
	return true;
}

void ULobbyWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (UWorld* World = GetWorld())
	{
		CachedGameState = World->GetGameState<ABlasterGameState>();
		if (CachedGameState)
		{
			CachedGameState->OnVoteCountsChanged.AddDynamic(this, &ULobbyWidget::HandleVoteCountsChanged);

			HandleVoteCountsChanged(CachedGameState->VoteCounts);
		}
	}
}

void ULobbyWidget::NativeDestruct()
{
	if (CachedGameState)
	{
		CachedGameState->OnVoteCountsChanged.RemoveDynamic(this, &ULobbyWidget::HandleVoteCountsChanged);
		CachedGameState = nullptr;
	}
	MenuTeardown();
	Super::NativeDestruct();
}

void ULobbyWidget::LeftMapButtonClicked()
{
	LastVote = 0;
	ButtonCastVote(0);
}
void ULobbyWidget::RightMapButtonClicked()
{
	LastVote = 1;
	ButtonCastVote(1);
}

void ULobbyWidget::ButtonCastVote(int32 Vote)
{
	Cast<ABlasterPlayerControllerBase>(GetOwningPlayer())->CastVote(Vote);
}

void ULobbyWidget::DisplayVotedMapTick()
{
	if (LastVote == 1)
	{
		LeftVoteSelectionTickImage->SetVisibility(ESlateVisibility::Collapsed);
		RightVoteSelectionTickImage->SetVisibility(ESlateVisibility::Visible);
	} else if (LastVote == 0)
	{
		RightVoteSelectionTickImage->SetVisibility(ESlateVisibility::Collapsed);
		LeftVoteSelectionTickImage->SetVisibility(ESlateVisibility::Visible);
	}	
}

void ULobbyWidget::MenuTeardown()
{
	RemoveFromParent();
	TogglePlayerControllerWidgetInteraction(false);
}

void ULobbyWidget::TogglePlayerControllerWidgetInteraction(bool Activate)
{
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			if (Activate)
			{
				FInputModeGameAndUI InputModeData;
				InputModeData.SetWidgetToFocus(TakeWidget());
				InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
				PlayerController->SetInputMode(InputModeData);
				PlayerController->SetShowMouseCursor(true);
			} else
			{
				FInputModeGameOnly InputModeData;
				PlayerController->SetInputMode(InputModeData);
				PlayerController->SetShowMouseCursor(false);
			}
		}
	}
}

void ULobbyWidget::HandleVoteCountsChanged(const TArray<int32>& NewVoteCounts)
{
	int32 LeftVotes  = NewVoteCounts.IsValidIndex(0) ? NewVoteCounts[0] : 0;
	int32 RightVotes = NewVoteCounts.IsValidIndex(1) ? NewVoteCounts[1] : 0;

	LeftMapButton->SetIsEnabled(true);
	RightMapButton->SetIsEnabled(true);
	
	UpdateProgressBars(LeftVotes, RightVotes);
}