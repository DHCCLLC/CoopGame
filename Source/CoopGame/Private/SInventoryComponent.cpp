// Fill out your copyright notice in the Description page of Project Settings.

#include "SInventoryComponent.h"
#include "SEnums.h"
#include "SAmmunitionComponent.h"


// Sets default values for this component's properties
USInventoryComponent::USInventoryComponent()
{
	PistolAmmoComp = CreateDefaultSubobject<USAmmunitionComponent>(TEXT("PistolAmmoComp"));
	PistolAmmoComp->Init(EWEAPONAMMUNITIONTYPE::WAT_Pistol, 100.0f, 0.0f);

	RifleAmmoComp = CreateDefaultSubobject<USAmmunitionComponent>(TEXT("RifleAmmoComp"));
	RifleAmmoComp->Init(EWEAPONAMMUNITIONTYPE::WAT_Rifle, 100.0f, 0.0f);

	GrenadeAmmoComp = CreateDefaultSubobject<USAmmunitionComponent>(TEXT("GrenadeAmmoComp"));
	GrenadeAmmoComp->Init(EWEAPONAMMUNITIONTYPE::WAT_Grenade, 45.0f, .0f);
}

// Called when the game starts
void USInventoryComponent::BeginPlay()
{
	Super::BeginPlay();	
}

//Handles all ammunition changes. Adding ammo. Consuming ammo. Also called when weapons are switched to update HUD.
void USInventoryComponent::ConsumeAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionChange)
{
	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		RifleAmmoComp->Consume(AmmunitionChange);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		GrenadeAmmoComp->Consume(AmmunitionChange);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Pistol:
		PistolAmmoComp->Consume(AmmunitionChange);
		break;
	default:
		break;
	}

	HandleAmmunitionChange(AmmunitionType);
}

bool USInventoryComponent::AddAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float AmmunitionToAdd, bool bActiveWeapon)
{
	bool bAddedInventory = false;

	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		bAddedInventory = RifleAmmoComp->Add(AmmunitionToAdd);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		bAddedInventory = GrenadeAmmoComp->Add(AmmunitionToAdd);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Pistol:
		bAddedInventory = PistolAmmoComp->Add(AmmunitionToAdd);
		break;
	default:
		break;
	}

	if(bAddedInventory && bActiveWeapon)
		HandleAmmunitionChange(AmmunitionType);

	return bAddedInventory;
}

void USInventoryComponent::HandleAmmunitionChange(EWEAPONAMMUNITIONTYPE AmmunitionType)
{
	float AmmunitionCount = 0;

	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		AmmunitionCount = RifleAmmoComp->GetAmmunitionCount();
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		AmmunitionCount = GrenadeAmmoComp->GetAmmunitionCount();
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Pistol:
		AmmunitionCount = PistolAmmoComp->GetAmmunitionCount();
		break;
	default:
		break;
	}

	OnAmmunitionChanged.Broadcast(AmmunitionType, AmmunitionCount);
}

bool USInventoryComponent::HasSufficientAmmunition(EWEAPONAMMUNITIONTYPE AmmunitionType, float RequestedAmmunition)
{
	switch (AmmunitionType)
	{
	case EWEAPONAMMUNITIONTYPE::WAT_Rifle:
		return RifleAmmoComp->HasSufficientAmmunition(RequestedAmmunition);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Grenade:
		return GrenadeAmmoComp->HasSufficientAmmunition(RequestedAmmunition);
		break;
	case EWEAPONAMMUNITIONTYPE::WAT_Pistol:
		return PistolAmmoComp->HasSufficientAmmunition(RequestedAmmunition);
		break;
	default:
		return false;
		break;
	}
}
