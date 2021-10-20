#include "CAnimNotify_EndJump.h"
#include "Global.h"
#include "Character/CPlayer.h"
#include "Actions/CActionData.h"
#include "Actions/CAction.h"

FString UCAnimNotify_EndJump::GetNotifyName_Implementation() const
{
	return "EndJump";
}

void UCAnimNotify_EndJump::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	CheckNull(MeshComp);
	CheckNull(MeshComp->GetOwner());

	ACPlayer* player = Cast<ACPlayer>(MeshComp->GetOwner());
	CheckNull(player);

	player->OffJump();
}

