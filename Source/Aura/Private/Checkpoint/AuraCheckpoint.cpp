// Copyright Kickback Studio


#include "Checkpoint/AuraCheckpoint.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Interaction/PlayerInterface.h"
#include "Components/DecalComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Game/AuraGameInstance.h"

AAuraCheckpoint::AAuraCheckpoint(const FObjectInitializer& ObjectInitializer) 
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CheckpointMesh = CreateDefaultSubobject<UStaticMeshComponent>("CheckpointMesh");
	CheckpointMesh->SetupAttachment(GetRootComponent());
	CheckpointMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CheckpointMesh->SetCollisionResponseToAllChannels(ECR_Block);

	CheckpointMesh->SetCustomDepthStencilValue(CustomDepthStencilValue);

	const float CapsuleHalfHeight = GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	CheckpointMesh->SetRelativeLocation(FVector(225.f, 0.f, -CapsuleHalfHeight));

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(CheckpointMesh);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	MoveToComponent = CreateDefaultSubobject<USceneComponent>("MoveToComponent");
	MoveToComponent->SetupAttachment(GetRootComponent());
}

void AAuraCheckpoint::LoadActor_Implementation()
{
	if (bReached)
	{
		OnReachedDelegate.Broadcast();
	}
}

void AAuraCheckpoint::HighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(true);
}

void AAuraCheckpoint::UnHighlightActor_Implementation()
{
	CheckpointMesh->SetRenderCustomDepth(false);
}

void AAuraCheckpoint::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	OutDestination = MoveToComponent->GetComponentLocation();
}

void AAuraCheckpoint::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bCanGlow && OtherActor->Implements<UPlayerInterface>())
	{
		bReached = true;
		OnReachedDelegate.Broadcast();

		if (UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetGameInstance()))
		{
			const UWorld* World = GetWorld();
			FString MapName = World->GetMapName();
			MapName.RemoveFromStart(World->StreamingLevelsPrefix);

			AuraGameInstance->SaveWorldState(GetWorld(), MapName);
		}

		IPlayerInterface::Execute_SaveProgress(OtherActor, PlayerStartTag);
		Glow();
	}
}

void AAuraCheckpoint::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraCheckpoint::OnSphereOverlap);
}

void AAuraCheckpoint::Glow()
{
	bCanGlow = false;
	UMaterialInstanceDynamic* DynamicMaterialInstance = CheckpointMesh->CreateAndSetMaterialInstanceDynamic(0);
	StartGlowTimeline(DynamicMaterialInstance);
}

void AAuraCheckpoint::EndGlow()
{
	bCanGlow = true;
	
	TArray<AActor*> OverlappingActors;
	Sphere->GetOverlappingActors(OverlappingActors);

	for (AActor* Actor : OverlappingActors)
	{
		if (Actor && Actor->ActorHasTag("Player"))
		{
			Glow();
			break;
		}
	}
}
