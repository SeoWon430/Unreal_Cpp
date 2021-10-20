
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSpawnObject.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOjbectBeginOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOjbectEndOverlap, class ACharacter*, InAttacker, class AActor*, InAttackCauser, class ACharacter*, InOtherCharacter);


// CAction_SpawnObject에서 스폰 될 액터
//	=> 충돌시 적에게 데미지를 줌
UCLASS()
class U02_CPP_API ACSpawnObject : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		float Duration = 5.0f;

	UPROPERTY(EditAnywhere)
		float Speed = 1000.0f;

	UPROPERTY(EditAnywhere)
		FVector StartPosition;

	UPROPERTY(EditAnywhere)
		FVector DestinationPosition;

	UPROPERTY(EditAnywhere)
		UParticleSystem* Effect;
	UPROPERTY(EditAnywhere)
		FTransform EffectTransform;

	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* Effect2;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;


protected:
	UPROPERTY(VisibleDefaultsOnly)
		class UCapsuleComponent* Collision;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* StaticMeshComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UNiagaraComponent* NiagaraComp;


	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		bool bMove;


public:	
	ACSpawnObject();


	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


	void Start(AActor* InLookActor = nullptr, bool IsOwnerLocation = true);

	UFUNCTION()
		void End();

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
	UPROPERTY(BlueprintAssignable)
		FOjbectBeginOverlap OnAttachmentBeginOverlap;

	UPROPERTY(BlueprintAssignable)
		FOjbectEndOverlap OnAttachmentEndOverlap;


private:
	//bool bMove;
	ACharacter* OwnerCharacter;
	FVector Direction;
	FVector Destination;
	//float Damage;
	//float LaunchPower;
};
