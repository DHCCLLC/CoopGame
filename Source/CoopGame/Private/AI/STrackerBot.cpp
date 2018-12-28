// Fill out your copyright notice in the Description page of Project Settings.

#include "AI/STrackerBot.h"
#include "Components/StaticMeshComponent.h"
#include "NavigationSystem.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "NavigationPath.h"
#include "DrawDebugHelpers.h"
#include "Components/SHealthComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/DamageType.h"
#include "Components/SphereComponent.h"
#include "SCharacter.h"
#include "Engine/EngineTypes.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values
ASTrackerBot::ASTrackerBot()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;
	MeshComp->SetCanEverAffectNavigation(false);
	MeshComp->SetSimulatePhysics(true);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetupAttachment(RootComponent);
	SphereComp->SetSphereRadius(200.0f);
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	bUseVelocityChange = true;
	MovementForce = 500.0f;
	RequiredDistanceToTarget = 100.0f;

	HealthComp = CreateDefaultSubobject<USHealthComponent>(TEXT("HealthComp"));

	bExploded = false;
	ExplosionRadius = 100.0f;
	ExplosionDamage = 40.0f;

	bStartedSelfDestruction = false;
	SelfDamageInterval = 0.25f;

	CheckPowerLevelInterval = 1.0f;

	BotDetectionRadius = 500.0f;

	MaxPowerLevel = 4;
}

// Called when the game starts or when spawned
void ASTrackerBot::BeginPlay()
{
	Super::BeginPlay();

	HealthComp->OnHealthChanged.AddDynamic(this, &ASTrackerBot::OnHealthChanged);

	if (Role == ROLE_Authority)
	{
		NextPathPoint = GetNextPathPoint();

		GetWorldTimerManager().SetTimer(TimerHandle_CheckPowerLevel, this, &ASTrackerBot::CheckPowerLevel, CheckPowerLevelInterval, true, 0.0f);
	}		
}

FVector ASTrackerBot::GetNextPathPoint()
{
	ACharacter* PlayerPawn = UGameplayStatics::GetPlayerCharacter(this, 0);

	UNavigationPath* NavPath = UNavigationSystemV1::FindPathToActorSynchronously(this, GetActorLocation(), PlayerPawn);

	if (NavPath->PathPoints.Num() > 1)
	{
		return NavPath->PathPoints[1];
	}

	return GetActorLocation();
}

void ASTrackerBot::OnHealthChanged(USHealthComponent * OwningHealthComponent, float Health, float HealthDelta, const UDamageType * DamageType, AController * InstigatedBy, AActor * DamageCauser)
{
	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("LastTimeDamageTaken", GetWorld()->TimeSeconds);
	}

	if (Health <= 0.0f)
	{
		SelfDestruct();
	}
}

void ASTrackerBot::SelfDestruct()
{
	if (bExploded)
		return;

	bExploded = true;

	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionEffect, GetActorLocation());

	UGameplayStatics::PlaySoundAtLocation(this, ExplodeSound, GetActorLocation());

	MeshComp->SetVisibility(false, true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	if (Role == ROLE_Authority)
	{
		//RadialForceComp->FireImpulse();

		TArray<AActor*>  IgnoreActors;
		IgnoreActors.Add(this);
		UGameplayStatics::ApplyRadialDamage(this, ExplosionDamage + (ExplosionDamage * PowerLevel), GetActorLocation(), ExplosionRadius, nullptr, IgnoreActors, this, GetInstigatorController(), true);// , ECollisionChannel::ECC_Visibility);

		SetLifeSpan(2.0f);
	}	
}

void ASTrackerBot::CheckPowerLevel()
{
	TArray < TEnumAsByte < EObjectTypeQuery> > ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_WorldDynamic));

	TArray <AActor*> Filter;
	Filter.Add(this);

	TArray <AActor*> OutActors;

	UKismetSystemLibrary::SphereOverlapActors(this, GetActorLocation(), BotDetectionRadius, ObjectTypes, ASTrackerBot::StaticClass(), Filter, OutActors);

	int32 BotsInRange = OutActors.Num();

	PowerLevel = (float)FMath::Clamp(BotsInRange, 0, MaxPowerLevel);

	float Alpha = PowerLevel / (float)MaxPowerLevel;
	//UE_LOG(LogTemp, Log, TEXT("Bot alpha: %f"), Alpha);

	if (MatInst == nullptr)
	{
		MatInst = MeshComp->CreateAndSetMaterialInstanceDynamicFromMaterial(0, MeshComp->GetMaterial(0));
	}

	if (MatInst)
	{
		MatInst->SetScalarParameterValue("PowerLevelAlpha", Alpha);
	}

	//UE_LOG(LogTemp, Log, TEXT("Bots in range: %i"), Actors);
	//
	//UE_LOG(LogTemp, Log, TEXT("Checking power level!"));
}

// Called every frame
void ASTrackerBot::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Role == ROLE_Authority && !bExploded)
	{
		float DistanceToTarget = (GetActorLocation() - NextPathPoint).Size();

		if (DistanceToTarget <= RequiredDistanceToTarget)
		{
			NextPathPoint = GetNextPathPoint();
		}
		else
		{
			FVector ForceDirection = NextPathPoint - GetActorLocation();
			ForceDirection.Normalize();

			ForceDirection *= MovementForce;

			MeshComp->AddForce(ForceDirection, NAME_None, bUseVelocityChange);

			DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), GetActorLocation() + ForceDirection, 32, FColor::Yellow, false, 0.0f, 0, 1.0f);
		}

		DrawDebugSphere(GetWorld(), NextPathPoint, 20, 12, FColor::Yellow, false, 0.0f, 2.0f);
	}
}

void ASTrackerBot::NotifyActorBeginOverlap(AActor * OtherActor)
{
	if (!bStartedSelfDestruction && !bExploded)
	{
		ASCharacter* PlayerPawn = Cast<ASCharacter>(OtherActor);

		if (PlayerPawn)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_SelfDamage, this, &ASTrackerBot::DamageSelf, SelfDamageInterval, true, 0.0f);

			bStartedSelfDestruction = true;

			UGameplayStatics::SpawnSoundAttached(SelfDestructSound, RootComponent, NAME_None, GetActorLocation(), EAttachLocation::KeepRelativeOffset);
		}
	}	
}

void ASTrackerBot::DamageSelf()
{
	UGameplayStatics::ApplyDamage(this, 20, GetInstigatorController(), this, nullptr);
}