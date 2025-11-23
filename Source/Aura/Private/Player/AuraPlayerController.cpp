// Copyright Kickback Studio


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "AuraGameplayTags.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h" // PgUp/PgDown stored as static function to adjust scalability. Could be moved.
#include "NiagaraSystem.h" // Click to move cursors
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include <Kismet/KismetSystemLibrary.h>
#include "Aura/Aura.h"
#include "Actor/MagicCircle.h"
#include "Interaction/CombatInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");

	CapsulePercentageForTrace = 1.0f;
	DebugLineTraces = true;
	IsOcclusionEnabled = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	if (IsValid(GetPawn()))
	{
		ActiveSpringArm = Cast<
			USpringArmComponent>(GetPawn()->GetComponentByClass(USpringArmComponent::StaticClass()));
		ActiveCamera = Cast<UCameraComponent>(GetPawn()->GetComponentByClass(UCameraComponent::StaticClass()));
		ActiveCapsuleComponent = Cast<UCapsuleComponent>(
			GetPawn()->GetComponentByClass(UCapsuleComponent::StaticClass()));
	}
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAction(ScalabilityAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::AdjustScalability);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();

	UpdateMagicCircleLocation();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::ShowMagicCircle()
{
	if (!IsValid(MagicCircle))
	{
		FVector MagicCircleLoc = CursorHit.ImpactPoint;
		MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass, MagicCircleLoc, FRotator::ZeroRotator);
	}
}

void AAuraPlayerController::HideMagicCircle()
{
	if (IsValid(MagicCircle))
	{
		MagicCircle->Destroy();
	}
}

void AAuraPlayerController::SyncOccludedActors()
{
	if (ShouldCheckCameraOcclusion())
	{
		if (ActiveSpringArm->bDoCollisionTest)
		{
			ForceShowOccludedActors();
		}
		else
		{
			FVector Start = ActiveCamera->GetComponentLocation();
			FVector End = GetPawn()->GetActorLocation();

			TArray<TEnumAsByte<EObjectTypeQuery>> CollisionObjectTypes;
			CollisionObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldStatic));

			TArray<AActor*> ActorsToIgnore;
			TArray<FHitResult> OutHits;

			auto ShouldDebug = DebugLineTraces ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None;

			bool bGotHits = UKismetSystemLibrary::CapsuleTraceMultiForObjects(
				GetWorld(), Start, End, ActiveCapsuleComponent->GetScaledCapsuleRadius() * CapsulePercentageForTrace,
				ActiveCapsuleComponent->GetScaledCapsuleHalfHeight() * CapsulePercentageForTrace, CollisionObjectTypes, true,
				ActorsToIgnore,
				ShouldDebug,
				OutHits, true);

			if (bGotHits)
			{
				// The list of actors hit by the line trace, that means that they are occluded from view
				TSet<const AActor*> ActorsJustOccluded;

				// Hide actors that are occluded by the camera
				for (FHitResult Hit : OutHits)
				{
					const AActor* HitActor = Hit.GetActor();
					HideOccludedActor(HitActor);
					ActorsJustOccluded.Add(HitActor);
				}

				// Show actors that are currently hidden but that are not occluded by the camera anymore 
				for (auto& Elem : OccludedActors)
				{
					if (!ActorsJustOccluded.Contains(Elem.Value.Actor) && Elem.Value.IsOccluded)
					{
						ShowOccludedActor(Elem.Value);

						if (DebugLineTraces)
						{
							UE_LOG(LogTemp, Warning,
								TEXT("Actor %s was occluded, but it's not occluded anymore with the new hits."), *Elem.Value.Actor->GetName());
						}
					}
				}
			}
			else
			{
				ForceShowOccludedActors();
			}
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	const FVector2D MovementAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.0f, Rotation.Yaw, 0.0f);

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, MovementAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, MovementAxisVector.X);
	}
}

void AAuraPlayerController::AdjustScalability(const FInputActionValue& InputActionValue)
{
	const float ScaleValue = InputActionValue.Get<float>();
	if (ScaleValue < 0.0f) // PgDn
	{
		UAuraAbilitySystemLibrary::AdjustScalability(false);
	}
	else if (ScaleValue > 0.0f) // PgUp
	{
		UAuraAbilitySystemLibrary::AdjustScalability(true);
	}
}

void AAuraPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_CursorTrace))
	{
		if (LastHoveredActor) LastHoveredActor->UnHighlightActor();
		if (HoveredActor) HoveredActor->UnHighlightActor();
		LastHoveredActor = nullptr;
		HoveredActor = nullptr;
		return;
	}
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	if (CursorHit.bBlockingHit)
	{
		LastHoveredActor = HoveredActor;
		HoveredActor = CursorHit.GetActor();

		if (LastHoveredActor != HoveredActor)
		{
			if (LastHoveredActor) LastHoveredActor->UnHighlightActor();
			if (HoveredActor) HoveredActor->HighlightActor();
		}
	}
}

void AAuraPlayerController::AutoRun()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		bAutoRunning = false;
		return;
	}

	if (bAutoRunning)
	{
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector ClosestLocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
			const FVector Direction = Spline->FindDirectionClosestToWorldLocation(ClosestLocationOnSpline, ESplineCoordinateSpace::World);
			ControlledPawn->AddMovementInput(Direction);

			const float DistanceToDestination = (ClosestLocationOnSpline - CachedDestination).Length();
			if (DistanceToDestination <= AutoRunAcceptanceRadius)
			{
				bAutoRunning = false;
			}
		}
	}
}

void AAuraPlayerController::UpdateMagicCircleLocation()
{
	if (IsValid(MagicCircle))
	{
		FVector MagicCircleLocation = FVector::ZeroVector;
		FVector LastValidLocation = MagicCircle->GetActorLocation();

		if (CursorHit.bBlockingHit)
		{
			if (CursorHit.GetActor() && CursorHit.GetActor()->Implements<UCombatInterface>())
			{
				MagicCircleLocation = ICombatInterface::Execute_GetCharacterLocationOnFloor(CursorHit.GetActor());
			}
			else
			{
				MagicCircleLocation = CursorHit.ImpactPoint;
			}
		}
		else
		{
			MagicCircleLocation = LastValidLocation;
		}

		MagicCircle->SetActorLocation(MagicCircleLocation);
	}
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}

	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = HoveredActor ? true : false;
		bAutoRunning = false;
	}
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}

	GetASC()->AbilityInputTagReleased(InputTag);

	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();

		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
			{
				if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
				{
					Spline->ClearSplinePoints();
					for (const FVector& PointLocation : NavPath->PathPoints)
					{
						Spline->AddSplinePoint(PointLocation, ESplineCoordinateSpace::World);
						/* DrawDebugSphere(
							GetWorld(),
							PointLocation,
							8.0f,
							8.0f,
							FColor::Green,
							false,
							5.0f
						); */
					}

					if (NavPath->PathPoints.Num() > 0)
					{
						CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
						bAutoRunning = true;


						if (FXCursor && GetASC() && !GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputPressed))
						{
							UNiagaraFunctionLibrary::SpawnSystemAtLocation(
								this,
								FXCursor,
								CachedDestination
							);
						}
					}
				}
			}
		}

		FollowTime = 0.0f;
		bTargeting = false;
	}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FAuraGameplayTags::Get().Player_Block_InputHeld))
	{
		return;
	}

	if ((bTargeting || bShiftKeyDown) && InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC() != nullptr)
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC() != nullptr)
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		if (CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

bool AAuraPlayerController::HideOccludedActor(const AActor* Actor)
{
	bool bSuccess = false;
	FCameraOccludedActor* ExistingOccludedActor = OccludedActors.Find(Actor);

	if (ExistingOccludedActor && ExistingOccludedActor->IsOccluded)
	{
		if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was already occluded. Ignoring."),
			*Actor->GetName());
		bSuccess = false;
	}

	if (ExistingOccludedActor && IsValid(ExistingOccludedActor->Actor))
	{
		ExistingOccludedActor->IsOccluded = true;
		ExistingOccludedActor->StaticMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);

		OnHideOccludedActor(*ExistingOccludedActor);

		if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s exists, but was not occluded. Occluding it now."), *Actor->GetName());
		bSuccess = true;
	}
	else
	{
		UStaticMeshComponent* StaticMesh = Cast<UStaticMeshComponent>(
			Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));

		StaticMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Ignore);

		FCameraOccludedActor OccludedActor;
		OccludedActor.Actor = Actor;
		OccludedActor.StaticMesh = StaticMesh;
		OccludedActor.Materials = StaticMesh->GetMaterials();
		OccludedActor.IsOccluded = true;
		OccludedActors.Add(Actor, OccludedActor);
		OnHideOccludedActor(OccludedActor);

		if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s does not exist, creating and occluding it now."), *Actor->GetName());
		bSuccess = true;
	}

	return bSuccess;
}

bool AAuraPlayerController::OnHideOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	for (int i = 0; i < OccludedActor.StaticMesh->GetNumMaterials(); ++i)
	{
		OccludedActor.StaticMesh->SetMaterial(i, FadeMaterial);
	}

	return true;
}

void AAuraPlayerController::ShowOccludedActor(FCameraOccludedActor& OccludedActor)
{
	if (!IsValid(OccludedActor.Actor))
	{
		OccludedActors.Remove(OccludedActor.Actor);
	}
	OccludedActor.StaticMesh->SetCollisionResponseToChannel(ECC_Target, ECR_Block);

	OccludedActor.IsOccluded = false;
	OnShowOccludedActor(OccludedActor);
}

bool AAuraPlayerController::OnShowOccludedActor(const FCameraOccludedActor& OccludedActor) const
{
	for (int i = 0; i < OccludedActor.Materials.Num(); ++i)
	{
		OccludedActor.StaticMesh->SetMaterial(i, OccludedActor.Materials[i]);
	}

	return true;
}

void AAuraPlayerController::ForceShowOccludedActors()
{
	for (TTuple<const AActor*, FCameraOccludedActor>& Elem : OccludedActors)
	{
		if (Elem.Value.IsOccluded)
		{
			ShowOccludedActor(Elem.Value);

			if (DebugLineTraces) UE_LOG(LogTemp, Warning, TEXT("Actor %s was occluded, force to show again."), *Elem.Value.Actor->GetName());
		}
	}
}

bool AAuraPlayerController::ShouldCheckCameraOcclusion() const
{
	return IsOcclusionEnabled && FadeMaterial && ActiveCamera && ActiveCapsuleComponent;
}
