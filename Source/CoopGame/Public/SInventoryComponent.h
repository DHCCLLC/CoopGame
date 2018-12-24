// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SEnums.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInventoryComponent.generated.h"

class USAmmunitionComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmunitionChangedSignature, EWEAPONAMMUNITIONTYPE, AmmunitionType, float, value);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USInventoryComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	USAmmunitionComponent* PistolAmmoComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	USAmmunitionComponent* RifleAmmoComp;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	USAmmunitionComponent* GrenadeAmmoComp;

public:

	UFUNCTION(BlueprintCallable, Category = "Player")
	void ConsumeAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionConsumed);

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool AddAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionToAdd, bool bActiveWeapon); //only adds if inventory is not full

	void HandleAmmunitionChange(EWEAPONAMMUNITIONTYPE AmmunitionType);

	bool HasSufficientAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float RequestedAmmunition);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmunitionChangedSignature OnAmmunitionChanged;

	//Placeholder until weapon pickup and weapon swapping is implemented
	void EquipWeapon(EWEAPONAMMUNITIONTYPE AmmunitionType) { ConsumeAmmunition(AmmunitionType, 0); }
};
