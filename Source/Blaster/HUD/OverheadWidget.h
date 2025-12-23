// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OverheadWidget.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* DisplayText;

	void SetDisplayText(FString TextToDisplay);

	UFUNCTION(BlueprintCallable)
	void ShowPlayerDisplayName(APawn* InPawn);
	
protected:
	virtual void NativeDestruct() override;

private:
	void TrySetPlayerName(APawn* InPawn);
	void ClearTimer();
	FTimerHandle PlayerStateTimer;
	int32 RetryCount = 0;
	static const int32 MaxRetries = 10;

};
