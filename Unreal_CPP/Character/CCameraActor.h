#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"
#include "CCameraActor.generated.h"

DECLARE_MULTICAST_DELEGATE(FMoveEnd);

// 특정 스킬에 사용될 카메라 무브
UCLASS()
class U02_CPP_API ACCameraActor : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;
	UPROPERTY(EditAnywhere)
		class UCurveFloat* Curve;

public:
	ACCameraActor();

public:
	UFUNCTION(BlueprintCallable)
		void StartCameraMove(class USplineComponent* InSpline, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket);

private:
	UFUNCTION()
		void OnProcess(float Output);	// 카메라 무빙

	UFUNCTION(BlueprintCallable)
		void OnEndProcess();			// 무빙 종료
		
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Reset();

public:
	FMoveEnd OnMoveEnd;

private:
	ACharacter* Target;
	FName TargetSocket;

	FTimeline Timeline;
	class USplineComponent* Spline;
	float FinishDelay;
};
