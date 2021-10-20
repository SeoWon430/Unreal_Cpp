#include "CCameraActor.h"
#include "Global.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Objects/CCameraPath.h"
#include "GameFramework/Character.h"


ACCameraActor::ACCameraActor()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Camera, "Camera");

	CHelpers::GetAsset<UCurveFloat>(&Curve, "CurveFloat'/Game/Camera/Curve_CameraPath.Curve_CameraPath'");
}


void ACCameraActor::BeginPlay()
{
	Super::BeginPlay();
}


void ACCameraActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Timeline.IsPlaying())
	{
		Timeline.TickTimeline(DeltaTime);


		if (!!Target)
		{
			FVector start = GetActorLocation();
			FVector target;
			if (TargetSocket.IsNone())
				target = Target->GetActorLocation();
			else
				target = Target->GetMesh()->GetSocketLocation(TargetSocket);
			SetActorRotation(UKismetMathLibrary::FindLookAtRotation(start, target));
		}
	}
}


void ACCameraActor::StartCameraMove(USplineComponent* InSpline, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket)
{
	Spline = InSpline;
	CheckNull(Spline);
	Target = InTarget;
	TargetSocket = InTargetSocket;
	FinishDelay = InDelay;


	FOnTimelineFloat process;
	process.BindUFunction(this, "OnProcess");

	FOnTimelineEvent finish;
	finish.BindUFunction(this, "OnEndProcess");

	Timeline = FTimeline();
	Timeline.AddInterpFloat(Curve, process);
	Timeline.SetTimelineFinishedFunc(finish);
	Timeline.SetPlayRate(InSpeed);
	Timeline.PlayFromStart();

	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	CheckNull(controller);
	controller->SetViewTarget(this);
}


void ACCameraActor::OnProcess(float Output)
{
	float length = Spline->GetSplineLength();

	FVector location = Spline->GetLocationAtDistanceAlongSpline(Output * length, ESplineCoordinateSpace::World);
	FRotator rotator = Spline->GetRotationAtDistanceAlongSpline(Output * length, ESplineCoordinateSpace::World);

	SetActorLocation(location);
	SetActorRotation(rotator);
}

void ACCameraActor::OnEndProcess()
{
	Spline = nullptr;
	Timeline.Stop();

	UKismetSystemLibrary::K2_SetTimer(this, "Reset", FinishDelay, false);

}

void ACCameraActor::Reset()
{
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	CheckNull(controller);

	controller->SetViewTarget(controller->GetPawn());

	if (OnMoveEnd.IsBound())
	{
		OnMoveEnd.Broadcast();
	}
}



