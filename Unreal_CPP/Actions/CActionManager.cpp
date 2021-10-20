#include "CActionManager.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "CAction.h"
#include "Weapon/CWeapon.h"

void UCActionManager::BeginPlay(class ACharacter* InOwnerCharacter, class UCActionData** OutAction)
{
	FTransform transform;


	ACAction* Action = nullptr;
	if (!!ActionClass)
	{
		Action = InOwnerCharacter->GetWorld()->SpawnActorDeferred<ACAction>(ActionClass, transform, InOwnerCharacter);
		Action->AttachToComponent(InOwnerCharacter->GetMesh(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

		//Action->SetDatas(ActionDatas);
		if(ActionDatas_Common.Num()>0)
			Action->SetDatas_Common(ActionDatas_Common);
		if (ActionDatas_Attack.Num() > 0)
			Action->SetDatas_Attack(ActionDatas_Attack);

		Action->SetActorLabel(GetLabelName(InOwnerCharacter, "Action"));
		UGameplayStatics::FinishSpawningActor(Action, transform);


	}

	// 플레이 시작시 읽어야 할 데이터를 넘겨줌
	//	=> 넘겨주는 데이터는 지역변수로 이전처럼 공유자원이 되지 않음 (덮어써지지 않음)
	//	=> ActionComponent에서 처리
	*OutAction = NewObject<UCActionData>();
	(*OutAction)->Action = Action;

}

FString UCActionManager::GetLabelName(ACharacter* InOwnerCharacter, FString InName)
{
	FString name;
	name.Append(InOwnerCharacter->GetActorLabel());
	name.Append("_");
	name.Append(InName);
	name.Append("_");
	name.Append(GetName().Replace(L"DA_", L""));

	return name;
}
