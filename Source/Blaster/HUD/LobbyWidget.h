// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

#include "LobbyWidget.generated.h"

class ABlasterGameState;

UCLASS()
class BLASTER_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void LobbyDisplaySetup();

	UFUNCTION(BlueprintCallable)
	void ToggleLobbyWidget(bool bShow);
	
	void UpdateProgressBars(int32 LeftMapVotes, int32 RightMapVotes);

	void DisplayVotedMapTick();
	
	UPROPERTY(meta = (BindWidget))
	UButton* LeftMapButton;

	UPROPERTY(meta = (BindWidget))
	UButton* RightMapButton;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* LeftMapProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* RightMapProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* ChooseMapTimer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* LeftMapProgressBarText;
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* RightMapProgressBarText;

	UPROPERTY(meta = (BindWidget))
	UImage* LeftVoteSelectionTickImage;

	UPROPERTY(meta = (BindWidget))
	UImage* RightVoteSelectionTickImage;
	
protected:

	virtual bool Initialize() override;
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleVoteCountsChanged(const TArray<int32>& NewVoteCounts);

	UPROPERTY()
	ABlasterGameState* CachedGameState;
	
private:
	
	UFUNCTION()
	void LeftMapButtonClicked();

	UFUNCTION()
	void RightMapButtonClicked();

	void MenuTeardown();

	FString PathToMap{TEXT("")};

	void TogglePlayerControllerWidgetInteraction(bool Activate);

	void ButtonCastVote(int32 vote);

	int32 LastVote;
};
