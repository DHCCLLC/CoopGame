// Fill out your copyright notice in the Description page of Project Settings.

#include "SAmmunitionComponent.h"

bool USAmmunitionComponent::Add(float AmmunitionToAdd)
{
	if (CurrentAmmunition == MaxAmmunition)
	{
		return false;
	}
	else
	{
		CurrentAmmunition = FMath::Clamp(CurrentAmmunition + AmmunitionToAdd, 0.0f, MaxAmmunition);

		return true;
	}
}
