
#include "CAnimNotifyState_WeaponAction.h"
#include "Global.h"
#include "Components/CActionComponent.h"
#include "Actions/CActionData.h"
#include "Actions/CAction_Combo.h"


FString UCAnimNotifyState_WeaponAction::GetNotifyName_Implementation() const
{
	return "WeaponAction";
}

void UCAnimNotifyState_WeaponAction::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(action);

	ACAction_Combo* attack = Cast<ACAction_Combo>(action->GetAction());
	CheckNull(attack);
	attack->WeaponAction();
}

void UCAnimNotifyState_WeaponAction::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(MeshComp->GetOwner());
	CheckNull(action);

	ACAction_Combo* attack = Cast<ACAction_Combo>(action->GetAction());
	CheckNull(attack);
	attack->WeaponActionEnd();
}
