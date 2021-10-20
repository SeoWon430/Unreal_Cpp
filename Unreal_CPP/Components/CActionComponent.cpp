#include "CActionComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Actions/CAction.h"
#include "Actions/CActionData.h"
#include "Actions/CActionManager.h"
#include "Actions/CAction_Build.h"
#include "Weapon/CWeapon.h"
#include "Character/CCameraActor.h"
#include "Components/SplineComponent.h"

UCActionComponent::UCActionComponent()
{

}


void UCActionComponent::BeginPlay()
{
	Super::BeginPlay();


	ACharacter* character = Cast<ACharacter>(GetOwner());
	
	// 무기 생성
	if (!!WeaponClass)
	{
		FTransform transform;
		Weapon = character->GetWorld()->SpawnActorDeferred<ACWeapon>(WeaponClass, transform, character);
		Weapon->SetActorLabel(character->GetActorLabel() + FString("_Weapon"));
		UGameplayStatics::FinishSpawningActor(Weapon, transform);
	}
	
	// 디테일에서 넣어준 DataAsset에서 데이터 가져오기
	for (int32 i = 0; i < (int32)EActionType_Common::Max; i++)
	{
		if (!!DataAssetsCommon[i])
			DataAssetsCommon[i]->BeginPlay(character, &DatasCommon[i]);
	}
	for (int32 i = 0; i < (int32)EActionType_Skill::Max; i++)
	{
		if (!!DataAssetsMelee[i])
			DataAssetsMelee[i]->BeginPlay(character, &DatasMelee[i]);
	}
	for (int32 i = 0; i < (int32)EActionType_Skill::Max; i++)
	{
		if (!!DataAssetsBow[i])
			DataAssetsBow[i]->BeginPlay(character, &DatasBow[i]);
	}


	// 특정 스킬에 사용할 카메라 무브 액터
	if (bUseCameraAction == true)
	{
		FTransform transform = FTransform();
		CameraActor = GetWorld()->SpawnActorDeferred<ACCameraActor>
			(
				ACCameraActor::StaticClass(),
				transform,
				GetOwner(),
				nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		UGameplayStatics::FinishSpawningActor(CameraActor, transform);
		CameraActor->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));

		TArray<UActorComponent*> actors;
		actors = GetOwner()->GetComponentsByClass(USplineComponent::StaticClass());
		for (UActorComponent* a : actors)
		{
			USplineComponent* spline = Cast<USplineComponent>(a);
			if (!!spline)
				CameraSplines.Add(spline);
		}
	}


	ActionType = EActionType::None;
	WeaponType = EWeaponType::Melee;

}

// 휴식
void UCActionComponent::Action_Rest()
{
	ChangeType(EActionType::Rest);
}

// 만들기
void UCActionComponent::Action_Build()
{
	ChangeType(EActionType::Build);
}

// 벽오르기&넘기 (벽타기와 다름)
void UCActionComponent::Action_Climb()
{
	ChangeType(EActionType::Climb);
}

// 와이어 던지기&날아가기
void UCActionComponent::Action_Wire()
{
	ChangeType(EActionType::Wire);
}

// 오브젝트 던지기
void UCActionComponent::Action_Throw()
{
	ChangeType(EActionType::Throw);
}

// 평타
void UCActionComponent::Action_Attack()
{
	ChangeType(EActionType::Attack);
}

// 스킬1~4
void UCActionComponent::Action_Skill1()
{
	ChangeType(EActionType::Skill1);
}
void UCActionComponent::Action_Skill2()
{
	ChangeType(EActionType::Skill2);
}
void UCActionComponent::Action_Skill3()
{
	ChangeType(EActionType::Skill3);
}
void UCActionComponent::Action_Skill4()
{
	ChangeType(EActionType::Skill4);
}
void UCActionComponent::UnAction_Skill4()
{
	ChangeType(EActionType::None);
}
void UCActionComponent::Action_Skill5()
{
	ChangeType(EActionType::Skill5);
}

// 행동방식 변경
void UCActionComponent::ChangeType(EActionType InNewType)
{
	/*
	if (Type == InNewType)
		return;
		*/

	EActionType prevType = ActionType;
	ActionType = InNewType;

	if (OnActionTypeChanged.IsBound())
		OnActionTypeChanged.Broadcast(prevType, InNewType);
	
	Action(InNewType);
}

// 행동 시작
void UCActionComponent::Action()
{
	
	if (ActionType == EActionType::None)
		ActionType = EActionType::Attack;

	int32 index = (int32)ActionType;
	
	CurrentAction = nullptr;
	if (index < (int32)EActionType_Common::Max)
		CurrentAction = DatasCommon[index]->GetAction();
	else
	{
		index -= (int32)EActionType_Common::Max;
		if (index < (int32)EActionType_Skill::Max)
		{	
			if (WeaponType == EWeaponType::Melee)
				CurrentAction = DatasMelee[index]->GetAction();
			else
				CurrentAction = DatasBow[index]->GetAction();
		}
	}
	CheckNull(CurrentAction)

	// 행동에 따른 시작점 변경
	switch (ActionType)
	{
		case EActionType::Attack:
			CurrentAction->Action();
			break;
		case EActionType::Build:
		case EActionType::Wire: 
		case EActionType::Throw: 
		case EActionType::Skill1:
		case EActionType::Skill2:
		case EActionType::Skill3:
			CurrentAction->Begin_Action();
			break;
	}
}


void UCActionComponent::Action(EActionType InType)
{
	int32 index = (int32)InType;
	CurrentAction = nullptr;
	if (index < (int32)EActionType_Common::Max)
		CurrentAction = DatasCommon[index]->GetAction();
	else
	{
		index -= (int32)EActionType_Common::Max;
		if (index < (int32)EActionType_Skill::Max)
		{
			if (WeaponType == EWeaponType::Melee)
			{
				CurrentAction = DatasMelee[index]->GetAction();
			}
			else
			{
				CurrentAction = DatasBow[index]->GetAction();
			}
		}
	}

	if (!!CurrentAction && CurrentAction->CanUse())
		CurrentAction->Action();
	else
		Reset();
}


// 특정 스킬에 사용될 카메라 무브 처리 시작
void UCActionComponent::StartCameraMove(FString InSplineName, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket)
{
	CheckFalse(bUseCameraAction);
	for (USplineComponent* spline : CameraSplines)
	{
		if (spline->GetName().Contains(InSplineName))
		{
			CameraActor->StartCameraMove(spline, InSpeed, InDelay, InTarget, InTargetSocket);
		}
	}
}

ACAction* UCActionComponent::GetAction()
{
	return CurrentAction;
}

ACWeapon* UCActionComponent::GetWeapon()
{
	return Weapon;
}

ACCameraActor* UCActionComponent::GetCameraActor()
{
	return CameraActor;
}

void UCActionComponent::AttachWeapon(FName InSocketName)
{
	if(InSocketName.IsEqual(""))
		InSocketName = CurrentAction->GetSocketName();
	Weapon->AttachTo(InSocketName);
}

void UCActionComponent::Reset()
{
	ActionType = EActionType::None;
	CurrentAction = nullptr;
}

void UCActionComponent::Dead()
{
	ActionType = EActionType::None;
	CurrentAction = nullptr;
}

void UCActionComponent::End_Dead()
{
	for (int32 i = 0; i < (int32)EActionType_Common::Max; i++)
	{
		if (!!DatasCommon[i])
			DatasCommon[i]->GetAction()->Destroy();
	}

	for (int32 i = 0; i < (int32)EActionType_Skill::Max; i++)
	{
		if (!!DatasMelee[i])
			DatasMelee[i]->GetAction()->Destroy();
	}

	for (int32 i = 0; i < (int32)EActionType_Skill::Max; i++)
	{
		if (!!DatasBow[i])
			DatasBow[i]->GetAction()->Destroy();
	}
}

void UCActionComponent::AbortByDamage()
{
	CheckNull(CurrentAction);
	CheckTrue(IsNoneMode());

	CurrentAction->Abort();

	if(!!Weapon)
		Weapon->AttachTo("Weapon_Holster");
}


TArray<float> UCActionComponent::CoolTimes()
{
	TArray<float> times;

	for (int32 i = 1; i < 5; i++)
	{
		times.Add(DatasMelee[i]->GetAction()->GetCurrentCoolTime());
	}

	return times;
}