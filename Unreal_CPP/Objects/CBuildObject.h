#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CBuildObject.generated.h"

// CAction_Build에서 사용 될 액터
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
	virtual void BuildSpawn();		// 키를 누르면 이 객체 생성(테스트로 설치 가능을 보여줌)
	virtual void BuildStart(ACharacter* InOwnerCharacter);		// 설치 가능 상태에서 클릭시 설치
	virtual void BuildFinish();		// 설치 완료

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

protected:
	class UMaterialInterface* OriginMaterial;
	class UMaterialInstanceDynamic* Material;

	class ACharacter* OwnerCharacter;
};
