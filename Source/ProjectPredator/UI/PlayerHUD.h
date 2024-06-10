// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "PlayerHUD.generated.h"

UCLASS()
class PROJECTPREDATOR_API APlayerHUD : public AHUD
{
	// TODO Make this class Blueprint only and gather the different UI Elements into global widgets
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
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
	
	UFUNCTION(BlueprintCallable)
	void ShowGameOver();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FLinearColor ReticleColor;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool ShouldDrawReticle = true;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* TakedownKeyWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* ConsumeKeyWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* LoadingAnimationWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* AmmoWidget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* BarsWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* HealthVignetteWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UUserWidget* GameOverWidget;
};
