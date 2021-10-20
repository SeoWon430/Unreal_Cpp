
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CThrowDestination.generated.h"

// CAction_Throw에서 사용 될 액터
//	=> 던지기 예상 최종 지점을 표시
//	=> 최종 지점의 면 방향에 따라 Decal을 그림
UCLASS()
class U02_CPP_API ACThrowDestination : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* MeshComp;	// 위치를 표시할 반구형 액터

	UPROPERTY(VisibleDefaultsOnly)
		class UDecalComponent* DecalComp;		// 위치를 표시 할 바닥


public:	
	ACThrowDestination();

	void SetVisible();
	void SetHide();

	// 닿는 면의 방향에 따라 Decal을 회전 시킴
	void SetDestination(FVector InLocation, FVector InNormal);	

protected:
	virtual void BeginPlay() override;


};
