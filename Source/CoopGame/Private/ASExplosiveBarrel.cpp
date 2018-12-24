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

		FVector BoostIntensity = FVector::UpVector * ExplosionImpulse;
		MeshComp->AddImpulse(BoostIntensity, NAME_None, true);

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

		MeshComp->SetMaterial(0, ExplodedMaterial);

		RadialForceComp->FireImpulse();
		
		const TArray < AActor * >  IgnoreActors;
		UGameplayStatics::ApplyRadialDamage(GetWorld(), 100.0f, GetActorLocation(), 500.0f, UDamageType::StaticClass(), IgnoreActors, this, GetInstigatorController(), false, ECollisionChannel::ECC_Visibility);
	}
}

