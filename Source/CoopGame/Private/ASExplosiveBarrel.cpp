// Fill out your copyright notice in the Description page of Project Settings.

#include "ASExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SHealthComponent.h"
#include "PhysicsEngine/RadialForceComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Materials/MaterialInterface.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"
#include "Engine//World.h"
#include "UnrealNetwork.h"


// Sets default values
AASExplosiveBarrel::AASExplosiveBarrel()
{
	bExploded = false;

	//scene component
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetCollisionObjectType(ECC_PhysicsBody);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>(TEXT("RadialForceComp"));
	RadialForceComp->Radius = 500.0f;
	RadialForceComp->SetupAttachment(MeshComp);
	RadialForceComp->bImpulseVelChange = true;
	RadialForceComp->bAutoActivate = false;
	RadialForceComp->bIgnoreOwningActor = true;

	//actor component
	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));
	HealthComp->OnHealthChanged.AddDynamic(this, &AASExplosiveBarrel::OnHealthChanged);

	ExplosionImpulse = 400.0f;

	SetReplicates(true);
	SetReplicateMovement(true);
}

// Called when the game starts or when spawned
void AASExplosiveBarrel::BeginPlay()
{
	Super::BeginPlay();
	
}

void AASExplosiveBarrel::OnHealthChanged(USHealthComponent * OwningHealthComp, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	UE_LOG(LogTemp, Log, TEXT("Health: %f"), Health);

	if (bExploded)
	{
		return;
	}		

	if (Health <= 0.0f)
	{
		bExploded = true;

		Explode();

		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		RadialForceComp->FireImpulse();

		const TArray < AActor * >  IgnoreActors;
		UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.0f, GetActorLocation(), 500.0f, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), false, ECollisionChannel::ECC_Visibility);
	}
}

void AASExplosiveBarrel::OnRep_Exploded()
{
	Explode();
}

void AASExplosiveBarrel::Explode()
{
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	MeshComp->SetMaterial(0, ExplodedMaterial);
}

void AASExplosiveBarrel::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AASExplosiveBarrel, bExploded);
}


