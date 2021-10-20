
#include "CWeapon_Spear.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "Components/CapsuleComponent.h"



ACWeapon_Spear::ACWeapon_Spear()
	:ACWeapon()
{
	NiagaraComp->SetRelativeLocation(FVector(300.0f, 0, 0));
	Collision->SetRelativeLocation(FVector(180.0f, 0, 0));
	Collision->SetCapsuleHalfHeight(180.0f);
	Collision->SetCapsuleRadius(22.0f);
}

void ACWeapon_Spear::Action(UINT InAttackIndex)
{
	OnCollision();
	Index = InAttackIndex;
	bAction = true;
	bMove = true;
	CurrentRange = 0;
	CurrentDuration = 0;

	NiagaraComp->SetVisibility(false);
	Collision->SetCapsuleRadius(AttackData[Index].CollisionRadius);


	// 각 평타에 따른 무기의 날아가는 방향(Direction) 설정
	switch (Index)
	{
		case 0:		// 평타1 : 전방으로 날림
			Direction = OwnerCharacter->GetActorForwardVector();
			break;
		case 1:		// 평타2 : 몽타주에서 오른손에 붙어 횡베기
			NiagaraComp->SetVisibility(true);
			return;
		case 2:		// 평타3 : 머리 위에서 내리 꽃음
			Direction = OwnerCharacter->GetActorForwardVector();
			Direction -= OwnerCharacter->GetActorUpVector() * 1.5f;
			Direction = Direction.GetSafeNormal();
			break;
		case 3:		// 평타4 : 전방으로 회전 시키며 날림
			Direction = OwnerCharacter->GetActorForwardVector();
			NiagaraComp->SetVisibility(true);
			break;
		default:
			ActionEnd();
			return;
	}


	FVector start = OwnerCharacter->GetMesh()->GetSocketLocation(AttackData[Index].StartSocketName);
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(start, start + Direction);

	SetActorLocation(start);
	SetActorRotation(rot);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

}

void ACWeapon_Spear::ActionEnd()
{
	CheckTrue(Index == -1);

	bAction = false;
	NiagaraComp->SetVisibility(false);
	OffCollision();

	if (AttackData[Index].EndAttachSocketName.IsNone() == false)
		AttachTo(AttackData[Index].EndAttachSocketName);
	
	if (Index == 3)
		SetActorRelativeRotation(FRotator(0, 0, 0));

	Index = -1;
}



void ACWeapon_Spear::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTrue(Index == -1);
	CheckTrue(Index == 1);		// 평타 2번은 무기를 날리지 않기에 생략
	CheckFalse(bAction);

	
	// 무기 독립 행동 유지 시간
	if (CurrentDuration > AttackData[Index].Duration)
	{
		ActionEnd();
		return;
	}
	else 
	{
		CurrentDuration += DeltaTime;
	}


	// 무기를 날림
	if (bMove)
	{
		FVector position = GetActorLocation();
		position += Direction * DeltaTime * AttackData[Index].Speed;
		SetActorLocation(position, true, &HitResult);
		/*
		if(!!HitResult.GetActor())
			CLog::Print(HitResult.GetActor()->GetName(), 8);
		else
			CLog::Print("No", 8);
			*/
		CurrentRange += DeltaTime * AttackData[Index].Speed;

		// 평타4만 무기를 회전 시킴
		if (Index == 3)
		{
			FRotator rot = FRotator(0);
			rot.Pitch += 3600.0f * DeltaTime;
			rot.Yaw = 0;
			rot.Roll = 0;
			AddActorLocalRotation(rot);
		}

		if (CurrentRange > AttackData[Index].Range)
			bMove = false;
	}


}