
#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "CAction_SpawnObject.generated.h"


// Ư�� ������Ʈ ���� (�÷��̾� ��ų 1,3)
//	=> �ش� ������Ʈ�� ������ �����ϼ� ����
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
	// ������ CSpawnObject�� ���ε� �Ǿ� Ÿ�� ó��
	void OnAttachmentBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;
	void OnAttachmentEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;


private:
	virtual void Abort() override;

private:

	TArray<class ACharacter*> HittedCharacters;
};
