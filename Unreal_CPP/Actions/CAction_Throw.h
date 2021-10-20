#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "Components/TimelineComponent.h"
#include "CAction_Throw.generated.h"

// 오브젝트 던지기 (Projectile)
//	=> PredictProjectilePath() 사용
UCLASS()
class U02_CPP_API ACAction_Throw : public ACAction
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(EditAnywhere)
		float ThrowPower = 1000.0f;

	UPROPERTY(EditAnywhere)
		int32 TrailCount = 15;

protected:
	virtual void BeginPlay() override;

public:
	ACAction_Throw();
	virtual void Action() override;
	virtual void Begin_Action() override;
	virtual void End_Action() override;

	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
		void OnThrowBeginOverlap(FHitResult InHitResult);

	UFUNCTION()
		void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	UFUNCTION()
		void Zooming(float Output);		// OnAim, OffAim 시 Timeline에 사용될 함수

	virtual void Abort() override;

	void OnAim();		// 던질 때 시야를 넓기
	void OffAim();		// 던지기 종료 후 원래 시야로 


private:
	class UNiagaraSystem* TrailNiagara;
	TArray<class UNiagaraComponent*> Trail;		// 던지기 예상 경로 표시

	class ACThrowDestination* DestinationEffect;	// 던지기 예상 최종위치 표시
	bool bThrow;

	FOccluderVertexArray VertexArray;		// PredictProjectilePath에 의해 나오는 예상 경로 위치들
	FVector Destination;

	class USpringArmComponent* SpringArm;
	FTimeline Timeline;
	FOnTimelineFloat TimelineFloat;
	class UCurveFloat* Curve;


	TSubclassOf<class ACThrowObject> ThrowObject;	// 던질 오브젝트
};
