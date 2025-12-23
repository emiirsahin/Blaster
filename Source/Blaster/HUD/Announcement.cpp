// Fill out your copyright notice in the Description page of Project Settings.


#include "Announcement.h"

void UAnnouncement::AnnouncementDisplaySetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Collapsed);
	SetIsFocusable(false);
}

void UAnnouncement::ToggleAnnouncement(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}


