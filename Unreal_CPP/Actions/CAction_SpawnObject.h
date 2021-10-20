
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "CAction_SpawnObject.generated.h"


// 특정 오브젝트 생성 (플레이어 스킬 1,3)
//	=> 해당 오브젝트는 스폰후 움직일수 있음
UCLASS()
class U02_CPP_API ACAction_SpawnObject : public ACAction
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACSpawnObject> SpawnObjectClass;

public:
	void Action() override;
	void Begin_Action() override;
	void End_Action() override;

	virtual void Tick(float DeltaTime) override;

public:
	// 생성한 CSpawnObject에 바인딩 되어 타격 처리
	void OnAttachmentBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;
	void OnAttachmentEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;


private:
	virtual void Abort() override;

private:

	TArray<class ACharacter*> HittedCharacters;
};
