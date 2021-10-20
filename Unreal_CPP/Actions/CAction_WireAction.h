
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction_Build.h"
#include "Components/CActionComponent.h"
#include "CAction_WireAction.generated.h"

// 와이어 던지기
//	=> CAction_Build을 상속받아 설치기능은 유사
//	=> 설치 후 키 재입력시 해당 설치 위치로 날아감 (설치는 벽에만 가능)
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
