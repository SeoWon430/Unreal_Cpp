
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Buff.generated.h"

// 버프 (플레이어 스킬2)
//	=> 일정 시간 CStatusComponent에 추가 스탯 설정
UCLASS()
class U02_CPP_API ACAction_Buff : public ACAction
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float RecoverHp;

	UPROPERTY(EditAnywhere)
		float RecoverStamina;

	UPROPERTY(EditAnywhere)
		float AddMotionSpeed;

	UPROPERTY(EditAnywhere)
		float AddAtk;

	UPROPERTY(EditAnywhere)
		float AddDef;

	UPROPERTY(EditAnywhere)
		float AddHitRecovery;


	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* StaticMeshComp;

protected:
	virtual void BeginPlay() override;


public:
	ACAction_Buff();
	virtual void Action() override;
	virtual void Begin_Action() override;
	virtual void End_Action() override;

private:
	UFUNCTION()
		virtual void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

	void StartBuff();

	UFUNCTION()
		void ResetBuff();

private:
	bool bBuff;
};
