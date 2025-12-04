// Copyright Kickback Studio


#include "Actor/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Components/SphereComponent.h"
#include "Interaction/EnemyInterface.h"

AMagicCircle::AMagicCircle()
{
 	PrimaryActorTick.bCanEverTick = true;

	TargetingSphere = CreateDefaultSubobject<USphereComponent>(TEXT("TargetingSphere"));
	SetRootComponent(TargetingSphere);
	TargetingSphere->SetCollisionResponseToChannels(ECR_Ignore);
	TargetingSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MagicCircleDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MagicCircleDecal"));
	MagicCircleDecal->SetupAttachment(RootComponent);
}

void AMagicCircle::BeginPlay()
{
	Super::BeginPlay();

	TargetingSphere->OnComponentBeginOverlap.AddDynamic(this, &AMagicCircle::OnTargetingSphereBeginOverlap);
	TargetingSphere->OnComponentEndOverlap.AddDynamic(this, &AMagicCircle::OnTargetingSphereEndOverlap);
}

void AMagicCircle::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (MagicCircleDecal)
	{
		if (!MagicCircleMID)
		{
			MagicCircleMID = MagicCircleDecal->CreateDynamicMaterialInstance();
		}
	}

	if (InitialRadius > 0.f)
	{
		SetTargetingRadius(InitialRadius);
	}
}

void AMagicCircle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMagicCircle::OnTargetingSphereBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->HighlightActor();
	}
}

void AMagicCircle::OnTargetingSphereEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IEnemyInterface* EnemyInterface = Cast<IEnemyInterface>(OtherActor))
	{
		EnemyInterface->UnHighlightActor();
	}
}

void AMagicCircle::SetTargetingRadius(float Radius)
{
	TargetingSphere->SetSphereRadius(Radius);

	InitialRadius = Radius;

	if (MagicCircleDecal)
	{
		MagicCircleDecal->DecalSize = FVector(Radius, Radius, MagicCircleDecal->DecalSize.Z);

		if (MagicCircleMID)
		{
			MagicCircleMID->SetScalarParameterValue(TEXT("Radius"), Radius);
		}
	}
}

