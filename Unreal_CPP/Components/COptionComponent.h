#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "COptionComponent.generated.h"

// 옵션 세팅용 
//	=> 마우스 감도
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class U02_CPP_API UCOptionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Camera")
		float HorizontalLookRate = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Camera")
		float VerticalLookRate = 45.0f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
		float ZoomSpeed = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Zoom")
		FVector2D ZoomRange = FVector2D(0, 500);

public:
	FORCEINLINE float GetHorizontalLookRate() { return HorizontalLookRate; }
	FORCEINLINE float GetVerticalLookRate() { return VerticalLookRate; }
	FORCEINLINE float GetZoomSpeed() { return ZoomSpeed; }
	FORCEINLINE FVector2D GetZoomRange() { return ZoomRange; }

public:
	UCOptionComponent();

protected:
	virtual void BeginPlay() override;


};
