// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryComponent.h"
#include "SEnums.h"


// Sets default values for this component's properties
USInventoryComponent::USInventoryComponent()
{
	GrenadeAmmunition = 0;
	RifleAmmunition = 0;
}


// Called when the game starts
void USInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void USInventoryComponent::HandleAmmunitionChange(EWEAPONAMMUNITIONTYPE AmmunitionType, float value, bool bActiveWeapon)
{
	float ActiveWeaponAmmunition = 0;

	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		RifleAmmunition += value;
		ActiveWeaponAmmunition = RifleAmmunition;
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		GrenadeAmmunition += value;
		ActiveWeaponAmmunition = GrenadeAmmunition;
		break;
	default:
		break;
	}

	if (bActiveWeapon)
	{
		OnAmmunitionChanged.Broadcast(AmmunitionType, ActiveWeaponAmmunition);
		//what about when we change weapons?
	}
}

bool USInventoryComponent::HasSufficientAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionCost)
{
	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		return ((RifleAmmunition - AmmunitionCost) >= 0);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		return ((GrenadeAmmunition - AmmunitionCost) >= 0);
		break;
	default:
		return false;
		break;
	}
}
