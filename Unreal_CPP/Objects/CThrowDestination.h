
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CThrowDestination.generated.h"

// CAction_Throw���� ��� �� ����
//	=> ������ ���� ���� ������ ǥ��
//	=> ���� ������ �� ���⿡ ���� Decal�� �׸�
UCLASS()
class U02_CPP_API ACThrowDestination : public AActor
{
	GENERATED_BODY()

protected:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* MeshComp;	// ��ġ�� ǥ���� �ݱ��� ����

	UPROPERTY(VisibleDefaultsOnly)
		class UDecalComponent* DecalComp;		// ��ġ�� ǥ�� �� �ٴ�


public:	
	ACThrowDestination();

	void SetVisible();
	void SetHide();

	// ��� ���� ���⿡ ���� Decal�� ȸ�� ��Ŵ
	void SetDestination(FVector InLocation, FVector InNormal);	

protected:
	virtual void BeginPlay() override;


};
