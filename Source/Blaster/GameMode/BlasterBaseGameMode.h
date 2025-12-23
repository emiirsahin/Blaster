
#pragma once

#include "CoreMinimal.h"
#include "BlasterTypes/MapVoteOption.h"
#include "GameFramework/GameMode.h"
#include "BlasterBaseGameMode.generated.h"

class ABlasterGameState;
class ABlasterPlayerState;


UCLASS()
class BLASTER_API ABlasterBaseGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABlasterBaseGameMode();
	
	UFUNCTION(BlueprintCallable)
	virtual void StartMapVote();

	UFUNCTION()
	virtual void EndMapVote();

	virtual void RegisterVote(int32 MapIndex, ABlasterPlayerState* VotingPlayerState);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Voting")
	TArray<FMapVoteOption> MapOptions;

	UPROPERTY()
	TArray<int32> VoteCounts;

	UPROPERTY(EditDefaultsOnly, Category="Voting")
	float SessionDuration;

	bool bIsVotingActive;

	FTimerHandle VotingTimerHandle;

	int32 WinningMapIndex;

	virtual void OnVoteCompleted(int32 ChosenMapIndex);

	ABlasterGameState* GetBlasterGameState() const;
};
