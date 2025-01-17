// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/SHealthComponent.h"
#include "Gameframework/Actor.h"
#include "UnrealNetwork.h"
#include "GameFramework/DamageType.h"


// Sets default values for this component's properties
USHealthComponent::USHealthComponent()
{
	DefaultHealth = 100.0f;

	SetIsReplicated(true);
}

// Called when the game starts
void USHealthComponent::BeginPlay()
{
	Super::BeginPlay();	

	//only hook if we are server
	if (GetOwnerRole() == ROLE_Authority)
	{
		AActor* MyOwner = GetOwner();

		if (MyOwner)
		{
			MyOwner->OnTakeAnyDamage.AddDynamic(this, &USHealthComponent::HandleTakeAnyDamage);
		}
	}	

	Health = DefaultHealth;
}

void USHealthComponent::HandleTakeAnyDamage(AActor * DamagedActor, float Damage, const class UDamageType * DamageType, class AController * InstigatedBy, AActor * DamageCauser)
{
	if (Damage <= 0.0f)
	{
		return;
	}

	Health = FMath::Clamp(Health - Damage, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, Damage, DamageType, InstigatedBy, DamageCauser);
}

void USHealthComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(this, Health, OldHealth - Health, GetDefault<UDamageType>() , nullptr, nullptr);
}

void USHealthComponent::Heal(float HealAmount)
{
	if (Health <= 0.0f || HealAmount <= 0.0f)
		return;

	Health = FMath::Clamp(Health + HealAmount, 0.0f, DefaultHealth);

	OnHealthChanged.Broadcast(this, Health, -HealAmount, nullptr, nullptr, nullptr);
}

void USHealthComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USHealthComponent, Health);
}
