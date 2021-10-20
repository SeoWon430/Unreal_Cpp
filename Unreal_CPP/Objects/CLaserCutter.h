#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CLaserCutter.generated.h"

// PrceduralMesh�� Slice�ϴ� ������
//	=> CAction_Cutter���� ���
//	=> �ٸ� ������Ʈ�� �ε����� �ݻ簢�� �°� �ݻ� ��
//	=> �������� Niagara�� ���� �������� ����
//		=> 0�� �������� �ε�����, 1�� �������� �����̰� ��
UCLASS()
class U02_CPP_API ACLaserCutter : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleDefaultsOnly)
		class UProjectileMovementComponent* Projectile;	// �̵��� ���� ������Ʈ

	UPROPERTY(VisibleDefaultsOnly)
		class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
		int32 ReflectCount = 3;		// �ݻ� ���� (3���� �������� 4�� ����)

	UPROPERTY(EditAnywhere)
		float Speed = 400.0f;

private:

	void SetLaser(UINT Index, UINT InSpawnProb, UINT InFireProb, FVector InStart, FVector InEnd, FVector InDirection, bool InVisible = true);


private:
	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	

public:	
	ACLaserCutter();

	UFUNCTION()
		void StartLaser();

	// �������� ����
	void Start(FVector InStart, FVector InDirection, class ACProceduralSkeletalMesh* InProcMesh, float InDelay = 0.0f);
	void MakeLaser();	// ������ ����

	void Hide();	// ������ ����
	void Stop();	// ������ ����

	// ������ ������ ����
	void ChangeDirection(FVector InDirection);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	class ACProceduralSkeletalMesh* Target;		// ������ ���ư��� �� Ÿ��

	class UNiagaraSystem* LaserNiagara;
	TArray<class UNiagaraComponent*> Lasers;	// ������ (���� ���� �������� ����)
	TArray<class ACEnemy*> Enemies;

	bool bHide;
	bool bLaser;
	float hitTime;
	bool bHit;
	bool bCut;
	float DamageTime;
	FVector StartLocation;
	FVector Direction;
	UINT CurrentIndex;		// ���ư� ������ ��ȣ

};
