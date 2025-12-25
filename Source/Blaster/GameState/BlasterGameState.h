
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BlasterTypes/MapVoteOption.h"
#include "BlasterGameState.generated.h"

UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
	GENERATED_BODY()

public:
	ABlasterGameState();

	UPROPERTY(Replicated)
	TArray<class ABlasterPlayerState*> TopScoringPlayers;
	void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category="Voting")
	TArray<FMapVoteOption> MapOptions;

	UPROPERTY(ReplicatedUsing=OnRep_VoteCounts, BlueprintReadOnly, Category="Voting")
	TArray<int32> VoteCounts;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnVoteCountsChanged, const TArray<int32>&, NewVoteCounts);

	UPROPERTY(BlueprintAssignable, Category="Voting")
	FOnVoteCountsChanged OnVoteCountsChanged;

	void UpdateVoteCounts(const TArray<int32>& NewVoteCounts);

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Voting")
	float VotingTimeRemaining;

	bool bIsVotingActive;
	
	int32 WinningMapIndex;

	void SetVotingState(bool bNewActive, float Duration, const TArray<FMapVoteOption>& NewMapOptions,
						const TArray<int32>& NewVoteCounts, int32 NewWinningIndex);
	
protected:
	UFUNCTION()
	void OnRep_VoteCounts();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	float TopScore = 0.f;
	
};
