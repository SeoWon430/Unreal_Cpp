#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CLaserCutter.generated.h"

// PrceduralMesh를 Slice하는 레이저
//	=> CAction_Cutter에서 사용
//	=> 다른 오브젝트에 부딪히면 반사각에 맞게 반사 됨
//	=> 여러개의 Niagara로 직선 레이저를 렌더
//		=> 0번 레이저가 부딪히면, 1번 레이저를 움직이게 함
UCLASS()
class U02_CPP_API ACLaserCutter : public AActor
{
	GENERATED_BODY()

private:

	UPROPERTY(VisibleDefaultsOnly)
		class UProjectileMovementComponent* Projectile;	// 이동을 위한 컴포넌트

	UPROPERTY(VisibleDefaultsOnly)
		class USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
		int32 ReflectCount = 3;		// 반사 갯수 (3개면 레이저는 4개 생성)

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

	// 레이저를 날림
	void Start(FVector InStart, FVector InDirection, class ACProceduralSkeletalMesh* InProcMesh, float InDelay = 0.0f);
	void MakeLaser();	// 레이저 생성

	void Hide();	// 레이저 숨김
	void Stop();	// 움직임 종료

	// 레이저 방향을 변경
	void ChangeDirection(FVector InDirection);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

private:
	class ACProceduralSkeletalMesh* Target;		// 강제로 날아가게 할 타겟

	class UNiagaraSystem* LaserNiagara;
	TArray<class UNiagaraComponent*> Lasers;	// 레이저 (각각 직선 레이저를 렌더)
	TArray<class ACEnemy*> Enemies;

	bool bHide;
	bool bLaser;
	float hitTime;
	bool bHit;
	bool bCut;
	float DamageTime;
	FVector StartLocation;
	FVector Direction;
	UINT CurrentIndex;		// 날아갈 레이저 번호

};
