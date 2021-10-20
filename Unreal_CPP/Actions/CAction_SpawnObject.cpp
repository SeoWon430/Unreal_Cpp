#include "CAction_SpawnObject.h"
#include "Global.h"
#include "Objects/CDamageBox.h"
#include "Objects/CEffectDestructible.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "GameFramework/Character.h"
#include "Interface/ISkillObject.h"
#include "Objects/CSpawnObject.h"


void ACAction_SpawnObject::Action()
{
	Super::Action();

	CheckFalse(Datas_Attack.Num() > 0);
	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetActionMode();

	PlayMontage_Attack(0);

	SetCoolTime();

	CameraAction(0, OwnerCharacter);
}


void ACAction_SpawnObject::Abort()
{
	End_Action();
}


void ACAction_SpawnObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


void ACAction_SpawnObject::Begin_Action()
{
	Super::Begin_Action();

	// 타격 처리 및 이펙트용 액터를 생성
	//	=> 이 액터에 적이 맞으면 데미지를 받도록 처리(델리게이트 이용)
	FTransform transform;
	transform = Datas_Attack[0].EffectTransform;
	ACSpawnObject* skill = GetWorld()->SpawnActorDeferred<ACSpawnObject>
		(
			SpawnObjectClass,
			transform,
			OwnerCharacter,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
	UGameplayStatics::FinishSpawningActor(skill, transform);
	skill->Start();

	skill->OnAttachmentBeginOverlap.AddDynamic(this, &ACAction_SpawnObject::OnAttachmentBeginOverlap);
	skill->OnAttachmentEndOverlap.AddDynamic(this, &ACAction_SpawnObject::OnAttachmentEndOverlap);

	HittedCharacters.Empty();
}


void ACAction_SpawnObject::End_Action()
{
	Super::End_Action();
}


void ACAction_SpawnObject::OnAttachmentBeginOverlap(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOtherCharacter)
{
	CheckNull(InOtherCharacter);

	for (const ACharacter* other : HittedCharacters)
	{
		if (InOtherCharacter == other)
			return;
	}
	HittedCharacters.Add(InOtherCharacter);


	//Send Damage
	FDamageEvent e;
	InOtherCharacter->TakeDamage(CalcDamage(Datas_Attack[0].Power), e, InAttacker->GetController(), InAttackCauser);

	// 피격 대상을 넉백 시킴
	FVector power = OwnerCharacter->GetActorForwardVector() * Datas_Attack[0].LaunchPower;
	InOtherCharacter->LaunchCharacter(power, true, false);
}


void ACAction_SpawnObject::OnAttachmentEndOverlap(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOtherCharacter)
{
	Super::OnAttachmentEndOverlap(InAttacker, InAttackCauser, InOtherCharacter);

}


