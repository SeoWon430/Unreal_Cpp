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

	// �÷��� ���۽� �о�� �� �����͸� �Ѱ���
	//	=> �Ѱ��ִ� �����ʹ� ���������� ����ó�� �����ڿ��� ���� ���� (��������� ����)
	//	=> ActionComponent���� ó��
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
