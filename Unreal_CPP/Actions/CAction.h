#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CActionManager.h"
#include "CAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionEnd);


// 모든 CAction_ㅁㅁㅁ 의 부모
/*	Action() : 키 입력시 실행 => Montage Play
*	Begin_Action() : 주로 노티파이에서 실행 => 특정 행동 실행
*	End_Action() : 주로 노티파이에서 실행 => 행동 종료
*/
UCLASS()
class U02_CPP_API ACAction : public AActor
{
	GENERATED_BODY()

public:
	// ActionComponent에서 처리한 DataAsset의 데이터 값 설정
	//	일반과 공격용 구조체 2가지 (쿨타임, 공격력 등 멤버가 다름)
	FORCEINLINE void SetDatas_Common(TArray<FActionData_Common> InDatas) { Datas_Common = InDatas; }
	FORCEINLINE void SetDatas_Attack(TArray<FActionData_Attack> InDatas) { Datas_Attack = InDatas; }


public:
	ACAction();

public:
	virtual void Action() {};			// 키입력으로 Action 시작 (몽타쥬 재생)
	virtual void Begin_Action() {};		// 몽타쥬 노티파이에서 실행
	virtual void End_Action();			// 몽타쥬 노티파이에서 실행
	virtual void Abort() {};			// Action 강제 종료

	bool CanUse();						// 쿨타임과 스테미너 고려하여 사용 가능 체크
	float GetCurrentCoolTime();			// 현재 남은 쿨타임
	float RatioCoolTime();				// 쿨타임 비율%
	virtual FName GetSocketName();		// 행동 진행중 소켓이 필요한 경우 사용

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
	void PlayMontage_Common(UINT InIndex);		// FActionData_Common의 몽타쥬 재생
	void PlayMontage_Attack(UINT InIndex);		// FActionData_Attack의 몽타쥬 재생
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
	TArray<FActionData_Common> Datas_Common;	// 일반용
	TArray<FActionData_Attack> Datas_Attack;	// 공격용 (쿨타임, 공격력 등 설정 가능)

	float coolTimeReset;

};
