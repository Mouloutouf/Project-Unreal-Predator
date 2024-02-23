// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUD.h"

#include "UMG/Public/Blueprint/UserWidget.h"
#include "UMG/Public/Components/SlateWrapperTypes.h"

APlayerHUD::APlayerHUD()
{
	SetReticleWhite();
}

void APlayerHUD::SetWidgetVisible(bool _Visible, UUserWidget* _Widget)
{
	ESlateVisibility Visibility = _Visible ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	_Widget->SetVisibility(Visibility);
}

void APlayerHUD::SetReticleRed()
{
	ReticleColor = FLinearColor(1, 0, 0, 1);
}

void APlayerHUD::SetReticleWhite()
{
	ReticleColor = FLinearColor(1, 1, 1, 1);
}
