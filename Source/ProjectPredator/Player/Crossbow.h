// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "GameFramework/Actor.h"
#include "Crossbow.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAmmoUpdatedSignature);

UCLASS()
class PROJECTPREDATOR_API ACrossbow : public AActor
{
	GENERATED_BODY() 
	
public:	
	// Sets default values for this actor's properties
	ACrossbow();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void LoadProjectile();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void CreateProjectile();
	
	UFUNCTION(BlueprintCallable)
	void TryReload();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReloadFirstPart();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void ReloadSecondPart();

	UFUNCTION(BlueprintCallable)
	void StartReload();
	
	UFUNCTION(BlueprintCallable)
	void ReloadProjectile();

	UFUNCTION(BlueprintCallable)
	void FinishReload();
	
	UFUNCTION(BlueprintCallable)
	void ShootProjectile(FVector _ClosestHitPosition);
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateAmmoUI();
	
public:	
	// Called every frame
	virtual void Tick(float _DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void TryShootProjectile(FVector _ClosestHitPosition);
	
	UFUNCTION(BlueprintCallable)
	void IncrementAmmo();

	UFUNCTION(BlueprintCallable)
	void DecrementAmmo();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* Root;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* ProjectileRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int StartAmmo = 1;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int CurrentAmmo = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	AProjectile* LoadedProjectile = nullptr;
	
	UPROPERTY(BlueprintAssignable, BlueprintReadOnly)
	FOnAmmoUpdatedSignature OnAmmoUpdated;
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool CanShoot = true;
};
