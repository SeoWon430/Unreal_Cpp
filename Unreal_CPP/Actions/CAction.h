#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CActionManager.h"
#include "CAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionEnd);


// ��� CAction_������ �� �θ�
/*	Action() : Ű �Է½� ���� => Montage Play
*	Begin_Action() : �ַ� ��Ƽ���̿��� ���� => Ư�� �ൿ ����
*	End_Action() : �ַ� ��Ƽ���̿��� ���� => �ൿ ����
*/
UCLASS()
class U02_CPP_API ACAction : public AActor
{
	GENERATED_BODY()

public:
	// ActionComponent���� ó���� DataAsset�� ������ �� ����
	//	�Ϲݰ� ���ݿ� ����ü 2���� (��Ÿ��, ���ݷ� �� ����� �ٸ�)
	FORCEINLINE void SetDatas_Common(TArray<FActionData_Common> InDatas) { Datas_Common = InDatas; }
	FORCEINLINE void SetDatas_Attack(TArray<FActionData_Attack> InDatas) { Datas_Attack = InDatas; }


public:
	ACAction();

public:
	virtual void Action() {};			// Ű�Է����� Action ���� (��Ÿ�� ���)
	virtual void Begin_Action() {};		// ��Ÿ�� ��Ƽ���̿��� ����
	virtual void End_Action();			// ��Ÿ�� ��Ƽ���̿��� ����
	virtual void Abort() {};			// Action ���� ����

	bool CanUse();						// ��Ÿ�Ӱ� ���׹̳� ����Ͽ� ��� ���� üũ
	float GetCurrentCoolTime();			// ���� ���� ��Ÿ��
	float RatioCoolTime();				// ��Ÿ�� ����%
	virtual FName GetSocketName();		// �ൿ ������ ������ �ʿ��� ��� ���

public:
	UFUNCTION()
		virtual void OnAttachmentBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) {};

	UFUNCTION()
		virtual void OnAttachmentEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) {};


	UPROPERTY(BlueprintAssignable)
		FActionEnd OnActionEnd;

protected:
	virtual void BeginPlay() override;

	float CalcDamage(float InPower);
	void PlayMontage_Common(UINT InIndex);		// FActionData_Common�� ��Ÿ�� ���
	void PlayMontage_Attack(UINT InIndex);		// FActionData_Attack�� ��Ÿ�� ���
	void SetCoolTime();

	void CameraAction(UINT Index, class ACharacter* InTarget);

private:
	void StartCameraMove(FString InSplineName, float InSpeed, float InDelay, class ACharacter* InTarget, FName InTargetSocket);
	void StartTimeDilation(float InSpeed);

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(BlueprintReadOnly)
		class ACharacter* OwnerCharacter;

	UPROPERTY(BlueprintReadOnly)
		class UCStateComponent* StateComp;

	UPROPERTY(BlueprintReadOnly)
		class UCStatusComponent* StatusComp;

	UPROPERTY(BlueprintReadOnly)
		class UCActionComponent* ActionComp;

protected:
	TArray<FActionData_Common> Datas_Common;	// �Ϲݿ�
	TArray<FActionData_Attack> Datas_Attack;	// ���ݿ� (��Ÿ��, ���ݷ� �� ���� ����)

	float coolTimeReset;

};
