
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Continuous.generated.h"

// ������ ���� (�÷��̾� ��ų 4)
//	=> �Է�Ű�� ���� ���� (CPlayer�� �Է¿��� ����)
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
	void Begin_Action() override;	// CSpawnEffect�� ����
	void End_Action() override;

private:
	UFUNCTION()
		void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

private:
	ACSpawnEffect* Effect;	// ������ ���� �������� �� ����
};
