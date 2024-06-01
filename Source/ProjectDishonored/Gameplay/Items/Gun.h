// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Gun.generated.h"

UENUM()
enum FiringMode { Single, Burst, Auto };

/**
 * 
 */
UCLASS()
class PROJECTDISHONORED_API AGun : public AWeapon
{
	GENERATED_BODY()
	
public:
	AGun();

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void TryFire();

	UFUNCTION()
	void Fire();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void FireFeedback();

	//
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* FireRoot;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool InfiniteAmmo = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int MaxAmmo;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentAmmo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Range;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpreadAngleDegrees = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float Damage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<FiringMode> DefaultFiringMode;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TEnumAsByte<FiringMode> CurrentFiringMode;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int BurstFireAmount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float FireRate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float ReloadTime;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector CurrentShootLocation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	AActor* CurrentShootTarget;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool IsShooting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentTimeSinceLastFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentFireCount;
	
public:
	UFUNCTION(BlueprintCallable)
	void SetShootTarget(AActor* _Target);
	UFUNCTION(BlueprintCallable)
	void SetShootLocation(FVector _Location);

	UFUNCTION(BlueprintCallable)
	void Shoot();

	UFUNCTION(BlueprintCallable)
	void StopShoot();
	
	UFUNCTION(BlueprintCallable)
	void Reload();

	UFUNCTION(BlueprintCallable)
	void ChangeFiringMode(FiringMode _NewFiringMode);
	UFUNCTION(BlueprintCallable)
	void ResetFiringMode();

	virtual void Tick(float _DeltaTime) override;

	//

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanShoot = true;
};
