#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBuildObject.generated.h"

// CAction_Build���� ��� �� ����
UCLASS()
class U02_CPP_API ACBuildObject : public AActor
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(VisibleDefaultsOnly)
		class UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;



public:	
	ACBuildObject();
	void BuildCheck(bool InCanBuild);
	virtual void BuildSpawn();		// Ű�� ������ �� ��ü ����(�׽�Ʈ�� ��ġ ������ ������)
	virtual void BuildStart(ACharacter* InOwnerCharacter);		// ��ġ ���� ���¿��� Ŭ���� ��ġ
	virtual void BuildFinish();		// ��ġ �Ϸ�

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	class UMaterialInterface* OriginMaterial;
	class UMaterialInstanceDynamic* Material;

	class ACharacter* OwnerCharacter;
};
