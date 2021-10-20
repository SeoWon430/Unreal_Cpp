#include "CAnimNotify_NextAction.h"
#include "Global.h"
#include "Components/CActionComponent.h"
#include "Actions/CAction.h"

FString UCAnimNotify_NextAction::GetNotifyName_Implementation() const
{
	return "NextAction";
}

void UCAnimNotify_NextAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* actionComp = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(actionComp);
	ACAction* action = actionComp->GetAction();
	CheckNull(action);
	action->Begin_Action();
}
