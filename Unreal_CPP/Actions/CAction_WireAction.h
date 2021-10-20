
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction_Build.h"
#include "Components/CActionComponent.h"
#include "CAction_WireAction.generated.h"

// ���̾� ������
//	=> CAction_Build�� ��ӹ޾� ��ġ����� ����
//	=> ��ġ �� Ű ���Է½� �ش� ��ġ ��ġ�� ���ư� (��ġ�� ������ ����)
UCLASS()
class U02_CPP_API ACAction_WireAction : public ACAction_Build
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	virtual void Action() override;
	virtual void Begin_Action() override;
	virtual void End_Action() override;

	virtual void Tick(float DeltaTime) override;

private:
	void ActionWire(float DeltaTime);
	
private:
	virtual void AbortByTypeChange(EActionType InPrevType, EActionType InNewType) override;

	virtual void Abort() override;
	
private:
	bool bStartWireAction;
	float TargetLength;
	float Speed;

	AActor* Target;
	FVector StartLocation;
	FVector MoveRightVector;
};
