
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSceneCapture2D.generated.h"


// ĳ���Ͱ� ������ ������ ȭ�� ���� ȿ��
//	=> �÷��̾� ī�޶� �տ� �����Ǹ� ȭ��ĸ�ĸ� DestructMesh�� ������ ����
UCLASS()
class U02_CPP_API ACSceneCapture2D : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UDestructibleComponent* DestructMeshComp;	// ȭ�� ���� ȿ���� ���� ������Ʈ


public:
	ACSceneCapture2D();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void End();
};
