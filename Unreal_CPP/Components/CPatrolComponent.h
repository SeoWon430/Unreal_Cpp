#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CPatrolComponent.generated.h"


// AI에서 사용될 Patrol기능 (Spline지정시 이를 따라 감)
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U02_CPP_API UCPatrolComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		class ACPatrolPath* Path;

	UPROPERTY(EditAnywhere)
		int32 Index;

	UPROPERTY(EditAnywhere)
		bool bReverse;

	UPROPERTY(EditAnywhere)
		float AcceptanceRadius = 10.0f;

public:
	FORCEINLINE bool IsValid() { return Path != nullptr; }

public:
	UCPatrolComponent();

	bool GetMoveTo(FVector& OutLocation, float& OutAcceptanceRadius);
	void UpdateNextIndex();

protected:
	virtual void BeginPlay() override;
};
