
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Build.generated.h"


// �����(Build)
//	=> ��ġ ���� ������ ������Ʈ ��ġ (�÷��̾� �ֺ��� ��ġ)
//	LineTrace�� ���� ��ġ ���� �Ǵ� => BuildTrace()
UCLASS()
class U02_CPP_API ACAction_Build : public ACAction
{
	GENERATED_BODY()


protected:
	virtual void BeginPlay() override;
	

public:
	ACAction_Build();
	virtual void Action() override;
	virtual void Begin_Action() override;
	virtual void End_Action() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		virtual void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

private:
	void BuildSpawn();	// �ൿ ���۽� ������Ʈ ����
	void BuildReset();	// �ൿ ����, ��ҽ� ������Ʈ ����
	void BuildTick();	// �ൿ �� ������Ʈ ��ġ ���� �Ǵ� => BuildTrace() ����
	void BuildTrace(FVector& OutLocation, bool& OutBuildAble);

protected:
	bool bBuildMode = false;
	bool CanBuild = true;
	class ACBuildObject* BuildObject;
	FVector BuildLocation;

	bool bRotation;
};
