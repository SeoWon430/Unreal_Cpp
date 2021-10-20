#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "Components/TimelineComponent.h"
#include "CAction_Throw.generated.h"

// ������Ʈ ������ (Projectile)
//	=> PredictProjectilePath() ���
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
		void Zooming(float Output);		// OnAim, OffAim �� Timeline�� ���� �Լ�

	virtual void Abort() override;

	void OnAim();		// ���� �� �þ߸� �б�
	void OffAim();		// ������ ���� �� ���� �þ߷� 


private:
	class UNiagaraSystem* TrailNiagara;
	TArray<class UNiagaraComponent*> Trail;		// ������ ���� ��� ǥ��

	class ACThrowDestination* DestinationEffect;	// ������ ���� ������ġ ǥ��
	bool bThrow;

	FOccluderVertexArray VertexArray;		// PredictProjectilePath�� ���� ������ ���� ��� ��ġ��
	FVector Destination;

	class USpringArmComponent* SpringArm;
	FTimeline Timeline;
	FOnTimelineFloat TimelineFloat;
	class UCurveFloat* Curve;


	TSubclassOf<class ACThrowObject> ThrowObject;	// ���� ������Ʈ
};
