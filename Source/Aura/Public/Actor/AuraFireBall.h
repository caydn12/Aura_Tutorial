// Copyright Kickback Studio

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"


UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:
	AAuraFireBall();
	
protected:
	virtual void BeginPlay() override;

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

	virtual void OnHit() override;

public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	UFUNCTION(BlueprintCallable)
	void EmptyHitActorsArray();

	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;

private:
	TArray<AActor*> HitActors;
};
