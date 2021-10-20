
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CBTService_Melee.generated.h"

// AI Controller에서 BehaviourTree의 경우를 판단하는 Service
//	=> 대기, 피격, 공격(Action), 순찰(Patrol), 추적의 경우를 판단
UCLASS()
class U02_CPP_API UCBTService_Melee : public UBTService
{
	GENERATED_BODY()

public:
	UCBTService_Melee();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
