#include "CAction_Buff.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Character/CPlayer.h"


ACAction_Buff::ACAction_Buff()
	:ACAction()
{
	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &StaticMeshComp, "StaticMeshComp", Scene);

	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StaticMeshComp->SetVisibility(false);
}


void ACAction_Buff::BeginPlay()
{
	Super::BeginPlay();

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Buff::AbortByTypeChange);
}


void ACAction_Buff::Action()
{
	Super::Action();

	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetActionMode();

	PlayMontage_Attack(0);

	SetCoolTime();
}


void ACAction_Buff::Begin_Action()
{
	Super::Begin_Action();
	StartBuff();
}


void ACAction_Buff::End_Action()
{
	Super::End_Action();
}


void ACAction_Buff::StartBuff()
{
	CheckTrue(bBuff);
	bBuff = true;
	
	// 추가스탯 설정
	StatusComp->AddMotionSpeed(AddMotionSpeed);
	StatusComp->AddHealth(RecoverHp);
	StatusComp->AddStamina(RecoverStamina);
	StatusComp->AddAtk(AddAtk);
	StatusComp->AddDef(AddDef);
	StatusComp->AddHitRecovery(AddHitRecovery);

	StaticMeshComp->SetVisibility(true);

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	if(!!player)
		player->UpdateStatusWidget();

	// 일정시간 이후 스탯 원상복귀
	UKismetSystemLibrary::K2_SetTimer(this, "ResetBuff", Duration, false);
}


void ACAction_Buff::ResetBuff()
{
	CheckFalse(bBuff);
	bBuff = false;
	StatusComp->AddMotionSpeed(-AddMotionSpeed);
	StatusComp->AddAtk(-AddAtk);
	StatusComp->AddDef(-AddDef);
	StatusComp->AddHitRecovery(-AddHitRecovery);

	StaticMeshComp->SetVisibility(false);
}


void ACAction_Buff::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Build && InPrevType != InNewType)
	{
		End_Action();
		//if (bBuff) ResetBuff();
	}
}


void ACAction_Buff::Abort()
{
	End_Action();
	//if (bBuff) ResetBuff();
}
