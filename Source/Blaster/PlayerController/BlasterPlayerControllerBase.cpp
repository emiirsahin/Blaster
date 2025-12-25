#include "BlasterPlayerControllerBase.h"
#include "HUD/BlasterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Character/BlasterCharacterBase.h"
#include "Components/VerticalBox.h"
#include "GameFramework/GameMode.h"
#include "Net/UnrealNetwork.h"
#include "HUD/Announcement.h"
#include "HUD/LobbyWidget.h"
#include "GameMode/BlasterBaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BlasterComponents/CombatComponent.h"
#include "GameState/BlasterGameState.h"
	
void ABlasterPlayerControllerBase::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABlasterCharacterBase* BlasterCharacter = Cast<ABlasterCharacterBase>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
	}
}

void ABlasterPlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());
	HUDInit();
	
	ServerCheckMatchState();
}

void ABlasterPlayerControllerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
}

void ABlasterPlayerControllerBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABlasterPlayerControllerBase, MatchState);
}

void ABlasterPlayerControllerBase::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerControllerBase::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;

	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;
	if (bHUDValid)
	{
		const float HealthPercentage = Health / MaxHealth;
		BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercentage);
		FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health),  FMath::CeilToInt(MaxHealth));
		BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
	}
}

void ABlasterPlayerControllerBase::SetHUDScore(float Score)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
		BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDScore = Score;
	}
}

void ABlasterPlayerControllerBase::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (bHUDValid)
	{
		FString DefeatsText = FString::Printf(TEXT("%d"), Defeats);
		BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
	}
	else
	{
		bInitializeCharacterOverlay = true;
		HUDDefeats = Defeats;
	}
}

void ABlasterPlayerControllerBase::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerControllerBase::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (bHUDValid)
	{
		FString AmmoText = FString::Printf(TEXT("%d"), Ammo);
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
	}
}

void ABlasterPlayerControllerBase::ClientAddToElimText_Implementation(const FString& AttackerName)
{
	SetElimText(AttackerName);
}

void ABlasterPlayerControllerBase::SetElimText(const FString& AttackerName)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ElimTextBox &&
		BlasterHUD->CharacterOverlay->AttackerName;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->ElimTextBox->SetVisibility(ESlateVisibility::Visible);
		BlasterHUD->CharacterOverlay->AttackerName->SetText(FText::FromString(AttackerName));
	}
}

void ABlasterPlayerControllerBase::ClearElimText()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ElimTextBox;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->ElimTextBox->SetVisibility(ESlateVisibility::Hidden);
	}
}

void ABlasterPlayerControllerBase::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerControllerBase::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;
	if (bHUDValid)
	{
		if (CountdownTime < 0.f)
		{
			BlasterHUD->Announcement->WarmupTime->SetText(FText());
			return;
		}
		
		int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
		int32 Seconds = CountdownTime - Minutes * 60;
		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerControllerBase::SetHUDTime()
{
	float TimeLeft = 0.f;
	if (MatchState == MatchState::Voting)
	{
		TimeLeft = FMath::Fmod(LevelStartingTime + VoteTime - GetServerTime() + WarmupTime + MatchTime + CooldownTime,  WarmupTime + MatchTime + CooldownTime);
	}
	else if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = LevelStartingTime + WarmupTime - GetServerTime();
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = LevelStartingTime + WarmupTime + MatchTime - GetServerTime();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = LevelStartingTime + WarmupTime + MatchTime + CooldownTime - GetServerTime();
	}
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);
	
	/*if (HasAuthority()) TODO idk if this is necessary.
	{
		BlasterBaseGameMode = BlasterBaseGameMode == nullptr ? Cast<ABlasterBaseGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterBaseGameMode;
		if (BlasterBaseGameMode)
		{
			SecondsLeft = FMath::CeilToInt(BlasterBaseGameMode->GetCountdownTime() + LevelStartingTime);
		}
	}*/
	
	if (CountdownInt != SecondsLeft)
	{
		if (MatchState == MatchState::Voting)
		{
			SetMapVoteCountdown(TimeLeft);
		}
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerControllerBase::SetMapVoteCountdown(float CountdownTime)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->LobbyWidget &&
		BlasterHUD->LobbyWidget->ChooseMapTimer;
	if (bHUDValid)
	{
		float ClampedTime = FMath::Max(0.f, CountdownTime);
		if (ClampedTime > VoteTime)
		{
			ClampedTime = 0.f;
		}
		
		int32 TotalSeconds = FMath::FloorToInt(ClampedTime);

		int32 Minutes = TotalSeconds / 60;
		int32 Seconds = TotalSeconds % 60;

		FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		BlasterHUD->LobbyWidget->ChooseMapTimer->SetText(FText::FromString(CountdownText));
	}
}

void ABlasterPlayerControllerBase::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
			}
		}
	}
}

void ABlasterPlayerControllerBase::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerControllerBase::ClientReportServerTime_Implementation(float TimeOfClientRequest,
	float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	float CurrentServerTime = TimeServerReceivedClientRequest + (0.5f * RoundTripTime);
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerControllerBase::GetServerTime() const
{
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerControllerBase::OnMatchStateSet(FName State)
{
	MatchState = State;
	
	if (MatchState == MatchState::Voting)
	{
		HandleVoting();
	}
	else if (MatchState == MatchState::WaitingToStart)
	{
		HandleMatchWaitingToStart();
	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerControllerBase::OnRep_MatchState()
{
	if (MatchState == MatchState::Voting)
	{
		HandleVoting();
	}
	else if (MatchState == MatchState::WaitingToStart)
	{
		HandleMatchWaitingToStart();
	}
	else if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerControllerBase::HandleMatchHasStarted()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->ToggleAnnouncement(false);
		}
	}
}

void ABlasterPlayerControllerBase::HandleMatchWaitingToStart()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->ToggleAnnouncement(true);
		}
	}
}

void ABlasterPlayerControllerBase::HandleCooldown()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;
		
		if (bHUDValid)
		{
			BlasterHUD->ToggleAnnouncement(true);
			FString AnnouncementText("Map Vote Starts In:");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString("What? Yall pacifists or sum?");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] == BlasterPlayerState)
				{
					InfoTextString = FString("You win");
				}
				else if (TopPlayers.Num() == 1 && TopPlayers[0] != BlasterPlayerState)
				{
					InfoTextString = FString::Printf(TEXT("Winner: \n%s"), *TopPlayers[0]->GetPlayerName());
				}
				else if (TopPlayers.Num() > 1)
				{
					InfoTextString = FString("Players tied for the win:\n");
					for (auto TiedPlayer : TopPlayers)
					{
						InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
					}
				}
				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
		}
	}
	ABlasterCharacterBase* BlasterCharacter = Cast<ABlasterCharacterBase>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat())
	{
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

void ABlasterPlayerControllerBase::HandleVoting()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->ToggleAnnouncement(false);
		}
		if (BlasterHUD->LobbyWidget)
		{
			BlasterHUD->ToggleLobbyWidget(true);
		}
	}
}

void ABlasterPlayerControllerBase::HUDInit()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->AddLobbyWidget();
		BlasterHUD->AddAnnouncement();
	}
}

void ABlasterPlayerControllerBase::ClientJoinMidGame_Implementation(FName StateOfMatch, float Warmup, float Match, float StartingTime, float Vote, float Cooldown)
{
	MatchState = StateOfMatch;
	WarmupTime = Warmup;
	MatchTime = Match;
	LevelStartingTime = StartingTime;
	VoteTime = Vote;
	CooldownTime = Cooldown;
	OnMatchStateSet(MatchState);
}

void ABlasterPlayerControllerBase::ServerCheckMatchState_Implementation()
{
	ABlasterBaseGameMode* GameMode = Cast<ABlasterBaseGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		VoteTime = GameMode->VoteTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		CooldownTime = GameMode->CooldownTime;
		OnMatchStateSet(GameMode->GetMatchState());
		ClientJoinMidGame(MatchState, WarmupTime, MatchTime, LevelStartingTime, VoteTime, CooldownTime);
	}
}

void ABlasterPlayerControllerBase::CastVote(int32 MapIndex)
{
	// This runs on client, forward to the server RPC.
	ServerCastVote(MapIndex);
}

void ABlasterPlayerControllerBase::ToggleLobbyWidget(bool bShow)
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->ToggleLobbyWidget(bShow);
	}
}

void ABlasterPlayerControllerBase::SetLobbyWidgetVoteTick()
{
	BlasterHUD = BlasterHUD == nullptr ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD && BlasterHUD->LobbyWidget)
	{
		BlasterHUD->LobbyWidget->DisplayVotedMapTick();
	}
}

void ABlasterPlayerControllerBase::SetIsVotingActive(bool bIsVoting)
{
	bIsVotingActive = bIsVoting;
}

void ABlasterPlayerControllerBase::ServerCastVote_Implementation(int32 MapIndex)
{
	ABlasterPlayerState* PS = GetPlayerState<ABlasterPlayerState>();
	if (!PS)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	ABlasterBaseGameMode* GM = World->GetAuthGameMode<ABlasterBaseGameMode>();
	if (!GM)
	{
		return;
	}
	GM->RegisterVote(MapIndex, PS);
}
