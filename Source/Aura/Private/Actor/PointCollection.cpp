// Copyright Kickback Studio


#include "Actor/PointCollection.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"

APointCollection::APointCollection()
{
	PrimaryActorTick.bCanEverTick = false;

	Pt_0 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_0"));
	ImmutablePoints.Add(Pt_0);
	SetRootComponent(Pt_0);

	Pt_1 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_1"));
	ImmutablePoints.Add(Pt_1);
	Pt_1->SetupAttachment(GetRootComponent());

	Pt_2 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_2"));
	ImmutablePoints.Add(Pt_2);
	Pt_2->SetupAttachment(GetRootComponent());

	Pt_3 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_3"));
	ImmutablePoints.Add(Pt_3);
	Pt_3->SetupAttachment(GetRootComponent());

	Pt_4 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_4"));
	ImmutablePoints.Add(Pt_4);
	Pt_4->SetupAttachment(GetRootComponent());

	Pt_5 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_5"));
	ImmutablePoints.Add(Pt_5);
	Pt_5->SetupAttachment(GetRootComponent());

	Pt_6 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_6"));
	ImmutablePoints.Add(Pt_6);
	Pt_6->SetupAttachment(GetRootComponent());

	Pt_7 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_7"));
	ImmutablePoints.Add(Pt_7);
	Pt_7->SetupAttachment(GetRootComponent());

	Pt_8 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_8"));
	ImmutablePoints.Add(Pt_8);
	Pt_8->SetupAttachment(GetRootComponent());

	Pt_9 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_9"));
	ImmutablePoints.Add(Pt_9);
	Pt_9->SetupAttachment(GetRootComponent());

	Pt_10 = CreateDefaultSubobject<USceneComponent>(TEXT("Pt_10"));
	ImmutablePoints.Add(Pt_10);
	Pt_10->SetupAttachment(GetRootComponent());
}

void APointCollection::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<USceneComponent*> APointCollection::GetGroundPoints(const FVector& GroundLocation, int32 NumPoints, float YawOverride)
{
	checkf(ImmutablePoints.Num() >= NumPoints, TEXT("Requested more points than available in PointCollection"));

	TArray<USceneComponent*> PointsCopy;

	for (USceneComponent* Point : ImmutablePoints)
	{
		if (PointsCopy.Num() >= NumPoints)
		{
			break;
		}

		// Rotate Points around Pt_0
		if (Point != Pt_0)
		{
			FVector ToPoint = Point->GetComponentLocation() - Pt_0->GetComponentLocation();
			ToPoint = ToPoint.RotateAngleAxis(YawOverride, FVector::UpVector);
			Point->SetWorldLocation(Pt_0->GetComponentLocation() + ToPoint);
		}

		const FVector RaisedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, GroundLocation.Z + 500.f);
		const FVector LoweredLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, GroundLocation.Z - 500.f);

		FHitResult HitResult;
		TArray<AActor*> IgnoreActors;
		UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(this, IgnoreActors, TArray<AActor*>(), 1500.f, Pt_0->GetComponentLocation());

		FCollisionQueryParams QueryParams;
		QueryParams.AddIgnoredActors(IgnoreActors);
		GetWorld()->LineTraceSingleByProfile(HitResult, RaisedLocation, LoweredLocation, FName("BlockAll"), QueryParams);

		const FVector AdjustedLocation = FVector(Point->GetComponentLocation().X, Point->GetComponentLocation().Y, HitResult.ImpactPoint.Z);

		// Point location at ground impact point
		Point->SetWorldLocation(AdjustedLocation);

		// Point rotation orthogonal (perpendicular) to the ground impact.
		Point->SetWorldRotation(UKismetMathLibrary::MakeRotFromZ(HitResult.ImpactNormal));

		PointsCopy.Add(Point);
	}

	return PointsCopy;
}

