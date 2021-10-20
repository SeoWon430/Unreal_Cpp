#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Climb.generated.h"

// �� ������&�ѱ�
//	=> ��Ÿ��� �޸� ���� �Ѿ�ų� �ö� ��
UCLASS()
class U02_CPP_API ACAction_Climb : public ACAction
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	ACAction_Climb();
	virtual void Action() override;
	virtual void Begin_Action() override;
	virtual void End_Action() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

private:
	FVector WallNormal;
	FVector EndLocation;
	bool bMove;
};
