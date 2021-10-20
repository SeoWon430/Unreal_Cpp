
#include "CAnimNotify_OnCollision.h"
#include "Global.h"
#include "Components/CActionComponent.h"
#include "Actions/CActionData.h"
#include "Actions/CAction.h"

FString UCAnimNotify_OnCollision::GetNotifyName_Implementation() const
{
	return "OnCollision";
}

void UCAnimNotify_OnCollision::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(action);
	ACAction* act = action->GetAction();
	CheckNull(act);
	act->OnCollision();
}

