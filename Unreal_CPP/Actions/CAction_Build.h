
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Build.generated.h"


// 만들기(Build)
//	=> 설치 가능 지역에 오브젝트 설치 (플레이어 주변에 설치)
//	LineTrace에 의해 설치 가능 판단 => BuildTrace()
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
	void BuildSpawn();	// 행동 시작시 오브젝트 생성
	void BuildReset();	// 행동 종료, 취소시 오브젝트 제거
	void BuildTick();	// 행동 중 오브젝트 설치 가능 판단 => BuildTrace() 실행
	void BuildTrace(FVector& OutLocation, bool& OutBuildAble);

protected:
	bool bBuildMode = false;
	bool CanBuild = true;
	class ACBuildObject* BuildObject;
	FVector BuildLocation;

	bool bRotation;
};
