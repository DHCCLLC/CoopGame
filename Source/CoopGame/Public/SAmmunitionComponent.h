// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SEnums.h"
#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAmmunitionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COOPGAME_API USAmmunitionComponent : public UActorComponent
{
	GENERATED_BODY()

protected:

	float MaxAmmunition;

	float CurrentAmmunition;

	EWEAPONAMMUNITIONTYPE EAmmunitionType;

public:
	USAmmunitionComponent() {}

	void Init(EWEAPONAMMUNITIONTYPE WAT, float MaxAmmo, float CurrentAmmo) { EAmmunitionType = WAT; MaxAmmunition = MaxAmmo; CurrentAmmunition = CurrentAmmo; }

	void Consume(float AmmunitionConsumed) { CurrentAmmunition -= AmmunitionConsumed; }

	float GetAmmunitionCount() { return CurrentAmmunition; }

	bool Add(float AmmunitionToAdd); //returns true if we are able to add

	bool HasSufficientAmmunition(float RequestedAmmunition) { return ((CurrentAmmunition - RequestedAmmunition) >= 0); }	
};
