// Fill out your copyright notice in the Description page of Project Settings.


#include "OverheadWidget.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerState.h"

void UOverheadWidget::SetDisplayText(FString TextToDisplay)
{
	if (DisplayText)
	{
		DisplayText->SetText(FText::FromString(TextToDisplay));
	}
}

void UOverheadWidget::ShowPlayerDisplayName(APawn* InPawn)
{
	if (!InPawn) 
	{
		ClearTimer();
		return;
	}
    
	// Clear any existing timer
	ClearTimer();
	RetryCount = 0;
    
	TrySetPlayerName(InPawn);
}

void UOverheadWidget::TrySetPlayerName(APawn* InPawn)
{
	if (!InPawn || RetryCount >= MaxRetries)
	{
		SetDisplayText(FString(TEXT("Player")));
		return;
	}

	if (APlayerState* PlayerState = InPawn->GetPlayerState())
	{
		SetDisplayText(FString(PlayerState->GetPlayerName()));
	}
	else
	{
		RetryCount++;
		GetWorld()->GetTimerManager().SetTimer(
			PlayerStateTimer,
			[this, InPawn]() { TrySetPlayerName(InPawn); },
			0.1f,
			false
		);
	}
}

void UOverheadWidget::ClearTimer()
{
	if (PlayerStateTimer.IsValid())
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(PlayerStateTimer);
		}
		PlayerStateTimer.Invalidate();
	}
}

void UOverheadWidget::NativeDestruct()
{
	ClearTimer();
	RemoveFromParent();
	Super::NativeDestruct();
}
