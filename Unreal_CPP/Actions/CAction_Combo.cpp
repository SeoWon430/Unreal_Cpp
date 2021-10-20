#include "CAction_Combo.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Objects/CDamageBox.h"
#include "Objects/CEffectDestructible.h"
#include "Weapon/CWeapon.h"


void ACAction_Combo::BeginPlay()
{
	Super::BeginPlay();

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Combo::AbortByTypeChange);


	// 이 Action은 콤보형으로 무기의 평타에 적용 됨
	//	=> 무기에서 충돌 발생 시 여기에서 처리 하도록 바인딩
	ACWeapon* weapon = ActionComp->GetWeapon();
	if (!!weapon)
	{
		weapon->OnAttachmentBeginOverlap.AddDynamic(this, &ACAction_Combo::OnAttachmentBeginOverlap);
		weapon->OnAttachmentEndOverlap.AddDynamic(this, &ACAction_Combo::OnAttachmentEndOverlap);
	}
}


void ACAction_Combo::Action()
{
	Super::Action();
	CheckFalse(Datas_Attack.Num() > 0);

	// 콤보 체크
	if (bEnable == true)
	{
		bExist = true;
		bEnable = false;
		return;
	}

	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetActionMode();

	// 피격 대상 초기화
	HittedCharacters.Empty();

	// 콤보 시작(1타)
	PlayMontage_Attack(0);
}


// 콤보 실행(2타 이후)
void ACAction_Combo::Begin_Action()
{
	Super::Begin_Action();

	CheckFalse(bExist);
	bExist = false;

	OwnerCharacter->StopAnimMontage();

	Index++;
	FMath::Clamp<int32>(Index, 0, Datas_Attack.Num() - 1);

	HittedCharacters.Empty();


	PlayMontage_Attack(Index);
}


void ACAction_Combo::WeaponAction()
{
	ActionComp->GetWeapon()->Action(Index);
}


void ACAction_Combo::WeaponActionEnd()
{
	ActionComp->GetWeapon()->ActionEnd();
}


void ACAction_Combo::End_Action()
{
	Super::End_Action();

	OwnerCharacter->StopAnimMontage(Datas_Attack[Index].AnimMontage);
	Index = 0;
}


void ACAction_Combo::OnAttachmentBeginOverlap(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOtherCharacter)
{
	//Super::OnAttachmentBeginOverlap(InAttacker, InAttackCauser, InOtherCharacter);

	CheckNull(InOtherCharacter);


	// 중복 공격을 막기 위해 피격대상 저장 및 체크
	for (const ACharacter* other : HittedCharacters)
	{
		if (InOtherCharacter == other)
			return;
	}
	HittedCharacters.Add(InOtherCharacter);


	// Play Effect Particle
	UParticleSystem* hitEffect = Datas_Attack[Index].Effect;
	if (!!hitEffect)
	{
		FTransform transform = Datas_Attack[Index].EffectTransform;
		transform.AddToTranslation(InOtherCharacter->GetActorLocation());
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitEffect, transform);
	}

	// Play Camera Shake
	TSubclassOf<UCameraShake> shake = Datas_Attack[Index].ShakeClass;
	if (!!shake)
	{
		APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (!!controller)
			controller->PlayerCameraManager->PlayCameraShake(shake);
	}

	// Send Damage
	FDamageEvent e;
	InOtherCharacter->TakeDamage(CalcDamage(Datas_Attack[Index].Power), e, InAttacker->GetController(), InAttackCauser);

}


void ACAction_Combo::OnAttachmentEndOverlap(ACharacter* InAttacker, AActor* InAttackCauser, ACharacter* InOtherCharacter)
{
	Super::OnAttachmentEndOverlap(InAttacker, InAttackCauser, InOtherCharacter);

}


void ACAction_Combo::ResetGlobalDilation()
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);

}


void ACAction_Combo::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Build && InPrevType != InNewType)
		End_Action();
}


void ACAction_Combo::Abort()
{
	End_Action();
}


FName ACAction_Combo::GetSocketName()
{
	return Datas_Attack[Index].SocketName;
}
