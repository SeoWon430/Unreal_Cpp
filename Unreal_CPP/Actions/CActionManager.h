#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CActionData.h"
#include "CActionManager.generated.h"

// ĳ����BP�� ActionComponent �����Ͽ��� DataAsset�� �־� �����͸� ����
UCLASS()
class U02_CPP_API UCActionManager : public UDataAsset
{
	GENERATED_BODY()


public:
	void BeginPlay(class ACharacter* InOwnerCharacter, class UCActionData** OutAction);

private:
	FString GetLabelName(class ACharacter* InOwnerCharacter, FString InName);

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TSubclassOf<class ACAction> ActionClass;		// ���� CAction Class (�ൿ ��� ����)

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FActionData_Common> ActionDatas_Common;		// �Ϲ� Action : �޽�, ��ġ, ���̾�

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FActionData_Attack> ActionDatas_Attack;		// ���� Action : ��Ÿ, ��ų1~4

};
