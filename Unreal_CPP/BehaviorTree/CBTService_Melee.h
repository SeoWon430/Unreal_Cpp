
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "CBTService_Melee.generated.h"

// AI Controller���� BehaviourTree�� ��츦 �Ǵ��ϴ� Service
//	=> ���, �ǰ�, ����(Action), ����(Patrol), ������ ��츦 �Ǵ�
UCLASS()
class U02_CPP_API UCBTService_Melee : public UBTService
{
	GENERATED_BODY()

public:
	UCBTService_Melee();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
