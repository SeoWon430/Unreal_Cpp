#include "CAction.h"
#include "Global.h"
#include "Character/CPlayer.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/CActionComponent.h"
#include "Objects/CDamageBox.h"
#include "Objects/CEffectDestructible.h"

ACAction::ACAction()
{
	PrimaryActorTick.bCanEverTick = true;
}


void ACAction::BeginPlay()
{
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	StateComp = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	StatusComp = CHelpers::GetComponent<UCStatusComponent>(OwnerCharacter);
	ActionComp = CHelpers::GetComponent<UCActionComponent>(OwnerCharacter);
	
	Super::BeginPlay();
}


void ACAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(coolTimeReset > 0.0f)
		coolTimeReset -= DeltaTime;

}


bool ACAction::CanUse()
{
	if (Datas_Attack.Num() > 0)
	{
		// ��Ÿ�Ӱ� ���׹̳� üũ
		if (coolTimeReset <= 0.0f
			&& StatusComp->GetStamina() >= Datas_Attack[0].Stamina)
			return true;
	}
	else
		return true;

	return false;
}

void ACAction::SetCoolTime()
{
	if (Datas_Attack.Num() > 0)
	{
		// ��Ÿ�Ӱ� ���׹̳� �Ҹ� ����
		coolTimeReset = Datas_Attack[0].CoolTime;
		StatusComp->AddStamina(-Datas_Attack[0].Stamina);

		// ���׹̳� ������ ���� ���� ������Ʈ
		ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
		if(!!player)
			player->UpdateStatusWidget();
	}
}


float ACAction::CalcDamage(float InPower)
{
	// �÷��̾��� ���ݷ�(StatusComp->GetAtk()) �� �׼��� ���ݷ�(Power)�� ���
	//	=> ���� ���ݷ� = �÷��̾� ���ݷ� * ���� �׼��� ���ݷ�(%)
	float result = InPower / 100.0f;
	result *= StatusComp->GetAtk();
	return result;
}


// ���� Action�� �Ϲ��϶� ����
void ACAction::PlayMontage_Common(UINT InIndex)
{
	OwnerCharacter->PlayAnimMontage(Datas_Common[InIndex].AnimMontage, Datas_Common[InIndex].PlayRate, Datas_Common[InIndex].StartSection);
	Datas_Common[InIndex].bCanMove ? StatusComp->SetMove() : StatusComp->SetStop();
}


// ���� Action�� ����(��ų)�϶� ����
void ACAction::PlayMontage_Attack(UINT InIndex)
{
	// �÷��̾��� �ӵ�(StatusComp->GetMotionSpeed()) �� ���ݸ�� PlayRate�� ����
	float playRate = Datas_Attack[InIndex].PlayRate;
	playRate *= StatusComp->GetMotionSpeed();
	OwnerCharacter->PlayAnimMontage(Datas_Attack[InIndex].AnimMontage, playRate, Datas_Attack[InIndex].StartSection);
	Datas_Attack[InIndex].bCanMove ? StatusComp->SetMove() : StatusComp->SetStop();
}


void ACAction::End_Action()
{
	StartTimeDilation(1.0f);
	StateComp->SetIdleMode();
	StatusComp->SetMove();
	ActionComp->Reset();

	if (OnActionEnd.IsBound())
		OnActionEnd.Broadcast();
}


void ACAction::CameraAction(UINT Index, ACharacter* InTarget)
{
	if (Datas_Attack[Index].CameraMoveSpeed > 0.0f)
	{
		StartCameraMove(Datas_Attack[Index].CameraPathName.ToString(), Datas_Attack[Index].CameraMoveSpeed, Datas_Attack[Index].CameraMoveStopTime, InTarget, Datas_Attack[Index].CameraTargetSocketName);
		StartTimeDilation(Datas_Attack[Index].TimeDilation);
	}
}


void ACAction::StartCameraMove(FString InSplineName, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket)
{
	ActionComp->StartCameraMove(InSplineName, InSpeed, InDelay, InTarget, InTargetSocket);
}


float ACAction::GetCurrentCoolTime()
{
	return coolTimeReset;
}


FName ACAction::GetSocketName()
{
	return Datas_Attack[0].SocketName;
}

float ACAction::RatioCoolTime()
{
	return coolTimeReset/Datas_Attack[0].CoolTime;
}

void ACAction::StartTimeDilation(float InSpeed)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), InSpeed);
}