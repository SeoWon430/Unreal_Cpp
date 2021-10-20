
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/TimelineComponent.h"
#include "CAction_Cutter.generated.h"

// 플레이어 스킬5
/*	=> ProceduralMesh를 이용한 Slice연출
*	=> 단계(Index)별로 실행
*		=> BeginPlay() : 노티파이에서 호출
*		=> NextAction : CCameraActor에서 카메라 무빙이 끝나면 델리게이트로 호출
*/
UCLASS()
class U02_CPP_API ACAction_Cutter : public ACAction
{
	GENERATED_BODY()
private:	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACProceduralSkeletalMesh> ProcClass;	// ProceduralMesh생성
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACSpawnObject> SpawnObjectClass;		// 레이저를 반사시키기 위한 액터
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACLaserCutter> LaserCutterClass;		// 6개를 스폰할 레이저(ProceduralMesh를 Slice)


	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* StartEffect;	// 행동 시작시 나올 이펙트
	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* EndEffect;	// 행동 종료 이펙트


	UPROPERTY(EditAnywhere)
		TArray<FName> CutSocketName;	// 처음 레이저가 자를 소켓
	UPROPERTY(EditAnywhere)
		TArray<FName> CutSocketName2;	// 반사된 레이저가 자를 소켓

	UPROPERTY(EditAnywhere)
		float StartHitFoward = 100.0f;	// 시작 타격 위치
	UPROPERTY(EditAnywhere)
		float StartHitRadius = 50.0f;

public:
	ACAction_Cutter();
	void Action() override;
	void Begin_Action() override;	// 노티파이에서 실행
	void End_Action() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// 단계별로 행동 실행	(카메라 움직임이 종료 되면 델리게이트로 실행)
	UFUNCTION()
		void NextAction();	

private:
	virtual void Abort() override;

	// 첫 단계에 적 피격시 적을 공중의로 띄움 (Timeline사용)
	UFUNCTION()
		void EnemyMoving(float Output);

	// 적 뒤에 6개 기둥을 차례로 기둥을 세움 (Timmer사용)
	UFUNCTION()
		void SpawnObject();

	// 적에게 6개의 레이져를 차례로 발사 (Timmer사용)
	UFUNCTION()
		void StartLaser();


	
private:
	class UNiagaraComponent* NiagaraComp;

	UINT Index;		// 현재 행동 단계
	class ACProceduralSkeletalMesh* ProcMesh;
	class ACEnemy* Target;
	class UAnimInstance* TargetAnim;
	TArray<FVector> TargetSocket;
	FVector TargetLocation;
	FVector FinishLocation;
	TArray<class ACLaserCutter*> Lasers;

	FTimeline Timeline;
	FOnTimelineFloat TimelineFloat;
	class UCurveFloat* Curve;
	FDelegateHandle CameraHandle;

	TArray<FVector> SpawnObjectLocation;
	UINT SpawnCount;
};
