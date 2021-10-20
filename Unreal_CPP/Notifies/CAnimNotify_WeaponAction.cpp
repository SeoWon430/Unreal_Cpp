
#include "CAnimNotify_WeaponAction.h"
#include "Global.h"
#include "Components/CActionComponent.h"
#include "Actions/CActionData.h"
#include "Actions/CAction_Combo.h"

FString UCAnimNotify_WeaponAction::GetNotifyName_Implementation() const
{
	return "WeaponAction";
}


void UCAnimNotify_WeaponAction::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(action);

	ACAction_Combo* attack = Cast<ACAction_Combo>(action->GetAction());
	CheckNull(attack);
	attack->WeaponAction();
}