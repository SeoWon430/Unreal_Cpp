
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSpawnEffect.generated.h"

DECLARE_MULTICAST_DELEGATE(FEffectEnd);

// CAction_Countinuous에서 사용 될 액터
//	=> Niagara를 지속하는 액터
//	=> 지속 데미지를 줌
UCLASS()
class U02_CPP_API ACSpawnEffect : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		float Duration = 5.0f;

	UPROPERTY(EditAnywhere)
		float Speed = 0.0f;

	UPROPERTY(EditAnywhere)
		float HitTime = 0.25f;

	UPROPERTY(EditAnywhere)
		FVector StartPosition;

	UPROPERTY(EditAnywhere)
		FVector DestinationPosition;


	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* Effect;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

protected:
	UPROPERTY(VisibleDefaultsOnly)
		class UCapsuleComponent* Collision;

	UPROPERTY(VisibleDefaultsOnly)
		class UNiagaraComponent* NiagaraComp;

public:	
	ACSpawnEffect();

	void Start(float InDamage);
	void End();

	UFUNCTION()
		void OnEnd();


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


public:
	FEffectEnd OnEndEvent;

private:
	ACharacter* OwnerCharacter;
	FVector Direction;
	FVector Destination;
	bool bMove;
	bool bEnd;

	float Damage;
	float HitTimer = 0.0f;
	TArray<class ACharacter*> HittedCharacters;
};
