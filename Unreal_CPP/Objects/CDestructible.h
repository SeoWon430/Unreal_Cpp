#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDestructible.generated.h"


// DestructibleMesh
//	=> 부서지는 메시
UCLASS()
class U02_CPP_API ACDestructible : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UDestructibleComponent* DestructMeshComp;

protected:
	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		virtual void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	
public:	
	ACDestructible();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

	void Initialize(float InSize);
	void Destruct(FVector InDirection, float Power = 2000.0f);


private:
	float Size;
	int32 DestructCount = 3;
	float DestructTime;
};
