#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCameraPath.generated.h"

// 카메라 움직임 경로
UCLASS()
class U02_CPP_API ACCameraPath : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		class UCurveFloat* Curve;

	UPROPERTY(VisibleDefaultsOnly)
		class USplineComponent* Spline;

public:
	FORCEINLINE class UCurveFloat* GetCurve() { return Curve; }
	FORCEINLINE class USplineComponent* GetSpline() { return Spline; }
	
public:	
	ACCameraPath();

};
