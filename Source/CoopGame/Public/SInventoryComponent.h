// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SEnums.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInventoryComponent.generated.h"

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
	float RifleAmmunition;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float GrenadeAmmunition;

public:

	void HandleAmmunitionChange(EWEAPONAMMUNITIONTYPE AmmunitionType, float value, bool bActiveWeapon = false);

	bool HasSufficientAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionCost);

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnAmmunitionChangedSignature OnAmmunitionChanged;

	//Placeholder until weapon pickup and eapon swapping is implemented
	void EquipWeapon(EWEAPONAMMUNITIONTYPE AmmunitionType) { HandleAmmunitionChange(AmmunitionType, 0, true); }
};
