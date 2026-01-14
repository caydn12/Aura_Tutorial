// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "Interaction/SaveInterface.h"
#include "Interaction/HighlightInterface.h"
#include "Aura/Aura.h"
#include "AuraCheckpoint.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReachedSignature);

class USphereComponent;
class UDecalComponent;
class UStaticMeshComponent;

UCLASS()
class AURA_API AAuraCheckpoint : public APlayerStart, public ISaveInterface, public IHighlightInterface
{
	GENERATED_BODY()
	
public:
	AAuraCheckpoint(const FObjectInitializer& ObjectInitializer);

	// Save Interface
	virtual bool ShouldLoadTransform_Implementation() override { return false; }
	virtual void LoadActor_Implementation() override;
	// End Save Interface

	// Highlight Interface
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;
	virtual void SetMoveToLocation_Implementation(FVector& OutDestination) override;
	// End Highlight Interface

	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

	UPROPERTY(BlueprintAssignable)
	FOnReachedSignature OnReachedDelegate;
protected:
	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	virtual void BeginPlay() override;

	void Glow();

	UFUNCTION(BlueprintImplementableEvent)
	void StartGlowTimeline(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintCallable)
	void EndGlow();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CheckpointMesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> MoveToComponent;

	UPROPERTY(EditDefaultsOnly)
	int32 CustomDepthStencilValue = CUSTOM_DEPTH_BLUE;
private:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	bool bCanGlow = true;
};
