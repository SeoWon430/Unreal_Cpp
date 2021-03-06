
#include "CBTTaskNode_Speed.h"
#include "Global.h"
#include "Character/CAIController.h"
#include "Character/CEnemy_AI.h"

UCBTTaskNode_Speed::UCBTTaskNode_Speed()
{
	NodeName = "Speed";
}

EBTNodeResult::Type UCBTTaskNode_Speed::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::ExecuteTask(OwnerComp, NodeMemory);

	ACAIController* controller = Cast<ACAIController>(OwnerComp.GetOwner());
	ACEnemy_AI* aiPawn = Cast<ACEnemy_AI>(controller->GetPawn());

	UCStatusComponent* status = CHelpers::GetComponent<UCStatusComponent>(aiPawn);
	status->SetSpeed(Type);

	return EBTNodeResult::Succeeded;
}

