
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Continuous.generated.h"

// 지속형 공격 (플레이어 스킬 4)
//	=> 입력키를 떼면 종료 (CPlayer의 입력에서 제어)
UCLASS()
class U02_CPP_API ACAction_Continuous : public ACAction
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACSpawnEffect> SpawnEffectClass;
	UPROPERTY(EditAnywhere)
		FName SocketName;

protected:
	virtual void BeginPlay() override;

public:
	void Action() override;
	void Begin_Action() override;	// CSpawnEffect를 스폰
	void End_Action() override;

private:
	UFUNCTION()
		void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

private:
	ACSpawnEffect* Effect;	// 적에게 지속 데미지를 줄 액터
};
