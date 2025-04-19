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

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
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

void AAuraPlayerController::CursorTrace()
{
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

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = HoveredActor ? true : false;
		bAutoRunning = false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	GetASC()->AbilityInputTagReleased(InputTag);

	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();

		if (FollowTime <= ShortPressThreshold && ControlledPawn)
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

				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}

		FollowTime = 0.0f;
		bTargeting = false;
		}
}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
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