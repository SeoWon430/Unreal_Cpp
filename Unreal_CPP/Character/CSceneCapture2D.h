
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CSceneCapture2D.generated.h"


// 캐릭터가 죽을때 나오는 화면 깨짐 효과
//	=> 플레이어 카메라 앞에 스폰되며 화면캡쳐를 DestructMesh에 입히고 깨짐
UCLASS()
class U02_CPP_API ACSceneCapture2D : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UDestructibleComponent* DestructMeshComp;	// 화면 깨짐 효과를 위한 컴포넌트


public:
	ACSceneCapture2D();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void End();
};
