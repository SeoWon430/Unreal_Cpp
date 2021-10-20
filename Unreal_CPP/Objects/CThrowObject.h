
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CThrowObject.generated.h"

// CAction_Throw에서 사용 될 액터
//	=> 실제 던져져 날아가는 액터
//	=> 곡선을 그리며 빠르게 날아가기 위해 CustomTimeDilation을 3배로 함
UCLASS()
class U02_CPP_API ACThrowObject : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(EditAnywhere)
		class UNiagaraSystem* Particle;
	

	UPROPERTY(VisibleDefaultsOnly)
		class UNiagaraComponent* NiagaraComp;

	UPROPERTY(VisibleDefaultsOnly)
		class USphereComponent* Collision;

	UPROPERTY(EditAnywhere)
		float Speed = 500.0f;

	UPROPERTY(EditAnywhere)
		float Range = 16.0f;

	UPROPERTY(EditAnywhere)
		float DamageRange = 200.0f;

	UPROPERTY(EditAnywhere)
		float Damage = 50.0f;


public:	
	ACThrowObject();

	void StartThrow(class ACharacter* InOwnerCharacter, float InPower, FVector InDestination, FRotator InDirection);

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		virtual void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	virtual void Tick(float DeltaTime) override;

private:
	void Impact();

	UFUNCTION()
		void End();

private:
	bool bMove;
	class ACharacter* OwnerCharacter;
	FVector PrevLocation;
	FVector Destination;

	TArray<ACharacter*> HittedCharacters;
};
