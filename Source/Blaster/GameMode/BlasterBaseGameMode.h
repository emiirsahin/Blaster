
#pragma once

#include "CoreMinimal.h"
#include "BlasterTypes/MapVoteOption.h"
#include "GameFramework/GameMode.h"
#include "PlayerState/BlasterPlayerState.h"    
#include "BlasterBaseGameMode.generated.h"

namespace MatchState
{
	extern BLASTER_API const FName Voting;	// The map vote is taking place.
	extern BLASTER_API const FName Cooldown; // Match duration has been reached. Display winner and begin cooldown timer.
	extern BLASTER_API const FName PreVote;
}

class ABlasterGameState;

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

	UPROPERTY(EditDefaultsOnly)
	float WarmupTime = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float LevelStartingTime = 0.f;

	UPROPERTY(EditDefaultsOnly)
	float MatchTime = 120.f;

	UPROPERTY(EditDefaultsOnly)
	float VoteTime = 20.f;

	UPROPERTY(EditDefaultsOnly)
	float CooldownTime = 10.f;

	FORCEINLINE float GetCountdownTime() const { return CountdownTime; }
	
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
	
	void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer);
	
	virtual void OnMatchStateSet() override;

	ABlasterGameState* GetBlasterGameState() const;

	float CountdownTime = 0.f;

	void StartVotingProcess();
};
