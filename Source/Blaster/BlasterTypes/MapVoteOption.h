#pragma once

#include "CoreMinimal.h"
#include "MapVoteOption.generated.h"

USTRUCT(BlueprintType)
struct BLASTER_API FMapVoteOption
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voting")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Voting")
	FName MapPath;
};
