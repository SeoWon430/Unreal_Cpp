#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CActionData.h"
#include "CActionManager.generated.h"

// 캐릭터BP의 ActionComponent 디테일에서 DataAsset을 넣어 데이터를 관리
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
		TSubclassOf<class ACAction> ActionClass;		// 사용될 CAction Class (행동 방식 정의)

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FActionData_Common> ActionDatas_Common;		// 일반 Action : 휴식, 설치, 와이어

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FActionData_Attack> ActionDatas_Attack;		// 공격 Action : 평타, 스킬1~4

};
