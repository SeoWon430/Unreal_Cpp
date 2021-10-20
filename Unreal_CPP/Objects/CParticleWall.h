
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CParticleWall.generated.h"

// Niagara Parameter로 파티클 수 제어
UCLASS()
class U02_CPP_API ACParticleWall : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly)
		class UNiagaraComponent* NiagaraComp;

	UPROPERTY(EditAnywhere)
		FVector ParticleCount = FVector(5);

	UPROPERTY(EditAnywhere)
		float ParticleSize = 100.0f;

public:	
	ACParticleWall();
	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	class ACharacter* Target;
};
