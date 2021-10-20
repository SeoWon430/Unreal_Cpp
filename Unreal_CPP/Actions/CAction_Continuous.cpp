#include "CAction_Continuous.h"
#include "Global.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "GameFramework/Character.h"
#include "Objects/CSpawnEffect.h"


void ACAction_Continuous::Action()
{
	Super::Action();

	CheckFalse(Datas_Attack.Num() > 0);
	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetActionMode();

	PlayMontage_Attack(0);
}


void ACAction_Continuous::BeginPlay()
{
	Super::BeginPlay();

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Continuous::AbortByTypeChange);
}


void ACAction_Continuous::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Skill4 && InPrevType != InNewType)
		End_Action();
}


void ACAction_Continuous::Abort()
{
	End_Action();
}


void ACAction_Continuous::Begin_Action()
{
	Super::Begin_Action();

	CheckFalse(Datas_Attack.Num() > 0);

	FTransform transform;
	transform.SetLocation(OwnerCharacter->GetMesh()->GetSocketLocation(SocketName));

	// 타격용 이펙트 생성
	transform.SetRotation(OwnerCharacter->GetActorRotation().Quaternion());
	Effect = GetWorld()->SpawnActorDeferred<ACSpawnEffect>
		(
			SpawnEffectClass,
			transform,
			OwnerCharacter,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
	UGameplayStatics::FinishSpawningActor(Effect, transform);
	Effect->Start(Datas_Attack[0].Power);
	Effect->OnEndEvent.AddUObject(this, &ACAction_Continuous::End_Action);
}


void ACAction_Continuous::End_Action()
{
	if(!!Effect)
		Effect->End();

	SetCoolTime();
	OwnerCharacter->StopAnimMontage(Datas_Attack[0].AnimMontage);
	Super::End_Action();
}
