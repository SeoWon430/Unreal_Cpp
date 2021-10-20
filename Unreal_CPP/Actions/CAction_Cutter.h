
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/TimelineComponent.h"
#include "CAction_Cutter.generated.h"

// �÷��̾� ��ų5
/*	=> ProceduralMesh�� �̿��� Slice����
*	=> �ܰ�(Index)���� ����
*		=> BeginPlay() : ��Ƽ���̿��� ȣ��
*		=> NextAction : CCameraActor���� ī�޶� ������ ������ ��������Ʈ�� ȣ��
*/
UCLASS()
class U02_CPP_API ACAction_Cutter : public ACAction
{
	GENERATED_BODY()
private:	
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACProceduralSkeletalMesh> ProcClass;	// ProceduralMesh����
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACSpawnObject> SpawnObjectClass;		// �������� �ݻ��Ű�� ���� ����
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACLaserCutter> LaserCutterClass;		// 6���� ������ ������(ProceduralMesh�� Slice)


	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* StartEffect;	// �ൿ ���۽� ���� ����Ʈ
	UPROPERTY(EditDefaultsOnly)
		class UNiagaraSystem* EndEffect;	// �ൿ ���� ����Ʈ


	UPROPERTY(EditAnywhere)
		TArray<FName> CutSocketName;	// ó�� �������� �ڸ� ����
	UPROPERTY(EditAnywhere)
		TArray<FName> CutSocketName2;	// �ݻ�� �������� �ڸ� ����

	UPROPERTY(EditAnywhere)
		float StartHitFoward = 100.0f;	// ���� Ÿ�� ��ġ
	UPROPERTY(EditAnywhere)
		float StartHitRadius = 50.0f;

public:
	ACAction_Cutter();
	void Action() override;
	void Begin_Action() override;	// ��Ƽ���̿��� ����
	void End_Action() override;

	virtual void Tick(float DeltaTime) override;

protected:
	virtual void BeginPlay() override;

public:
	// �ܰ躰�� �ൿ ����	(ī�޶� �������� ���� �Ǹ� ��������Ʈ�� ����)
	UFUNCTION()
		void NextAction();	

private:
	virtual void Abort() override;

	// ù �ܰ迡 �� �ǰݽ� ���� �����Ƿ� ��� (Timeline���)
	UFUNCTION()
		void EnemyMoving(float Output);

	// �� �ڿ� 6�� ����� ���ʷ� ����� ���� (Timmer���)
	UFUNCTION()
		void SpawnObject();

	// ������ 6���� �������� ���ʷ� �߻� (Timmer���)
	UFUNCTION()
		void StartLaser();


	
private:
	class UNiagaraComponent* NiagaraComp;

	UINT Index;		// ���� �ൿ �ܰ�
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
