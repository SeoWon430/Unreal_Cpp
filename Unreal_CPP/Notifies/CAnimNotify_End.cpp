#include "CAnimNotify_End.h"
#include "Global.h"
#include "Components/CActionComponent.h"
#include "Actions/CActionData.h"
#include "Actions/CAction.h"

FString UCAnimNotify_End::GetNotifyName_Implementation() const
{
	return "End";
}

void UCAnimNotify_End::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* actionComp = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(actionComp);

	if(!!actionComp->GetAction())
		actionComp->GetAction()->End_Action();
}