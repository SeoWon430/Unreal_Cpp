
#include "CCameraPath.h"
#include "Global.h"
#include "Components/SplineComponent.h"

ACCameraPath::ACCameraPath()
{
	CHelpers::CreateComponent(this, &Spline, "Spline");
	CHelpers::GetAsset<UCurveFloat>(&Curve, "CurveFloat'/Game/Camera/Curve_CameraPath.Curve_CameraPath'");

}

