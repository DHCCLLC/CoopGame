#pragma once

#include "SEnums.generated.h"

UENUM(BlueprintType)
enum class EWEAPONAMMUNITIONTYPE : uint8
{
	WAT_Rifle,
	WAT_Pistol,
	WAT_Grenade,
	WAT_Rocket
};

UENUM(BlueprintType)
enum class EWEAPONTYPE : uint8
{
	WT_Shotgun,
	WT_Pistol,
	WT_Rifle,
	WT_Grenade,
	WT_Rocket
};
