// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

/**
 * 
 */
UCLASS()
class PROJECTDISHONORED_API APlayerHUD : public AHUD
{
	GENERATED_BODY()

public:
	APlayerHUD();
	
	UFUNCTION(BlueprintCallable)
	void SetWidgetVisible(bool _Visible, UUserWidget* _Widget);

	UFUNCTION(BlueprintCallable)
	void SetReticleRed();
	UFUNCTION(BlueprintCallable)
	void SetReticleWhite();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateAmmoWidget(int _Value);
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FLinearColor ReticleColor;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* TakedownKeyWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* TakedownLoadWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* AmmoWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* HealthBarWidget;
};
