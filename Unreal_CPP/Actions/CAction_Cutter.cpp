#include "CAction_Cutter.h"
#include "Global.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/CActionComponent.h"
#include "GameFramework/Character.h"
#include "Objects/CProceduralSkeletalMesh.h"
#include "Character/CCameraActor.h"
#include "Character/CEnemy.h"
#include "Character/CPlayer.h"
#include "Objects/CSpawnObject.h"
#include "Objects/CLaserCutter.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


ACAction_Cutter::ACAction_Cutter()
	:ACAction()
{
	CHelpers::GetAsset<UCurveFloat>(&Curve, "CurveFloat'/Game/Actions/Curve_Rotation.Curve_Rotation'");

	TimelineFloat.BindUFunction(this, "EnemyMoving");
	Timeline.AddInterpFloat(Curve, TimelineFloat);
	Timeline.SetPlayRate(0.75f);
}


void ACAction_Cutter::BeginPlay()
{
	Super::BeginPlay();

	// 6���� ������ �̸� ����
	//	=> �ڸ� ��Ĺ�� Blueprint���� 6���� ������
	for (int32 i = 0; i < CutSocketName.Num(); i++)
	{
		FTransform transform = OwnerCharacter->GetTransform();
		ACLaserCutter* laser = GetWorld()->SpawnActorDeferred<ACLaserCutter>
			(
				LaserCutterClass,
				transform,
				OwnerCharacter,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		UGameplayStatics::FinishSpawningActor(laser, transform);
		Lasers.Add(laser);
		laser->MakeLaser();
		laser->Hide();
	}
}


void ACAction_Cutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	Timeline.TickTimeline(DeltaTime);
}


// SphereTrace�� ���� Ÿ�� �Ǹ� ���� �ܰ� ����
void ACAction_Cutter::Action()
{
	Super::Action();

	CheckFalse(Datas_Attack.Num() > 0);
	CheckFalse(StateComp->IsIdleMode());
	StateComp->SetActionMode();

	SetCoolTime();
	PlayMontage_Attack(0);
	CameraHandle = ActionComp->GetCameraActor()->OnMoveEnd.AddUObject(this, &ACAction_Cutter::NextAction);
	Index = 0;

	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery3);
	TArray<AActor*> ignores;
	ignores.Add(this);
	ignores.Add(OwnerCharacter);
	FVector start = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * StartHitFoward;
	FVector end = start + GetActorForwardVector();
	FHitResult result;

	FinishLocation = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorForwardVector() * StartHitFoward * 1.5f + OwnerCharacter->GetActorUpVector() * StartHitFoward * 0.5f;

	// ���� ����Ʈ ����
	if (!!StartEffect)
	{
		if(!!NiagaraComp)
			NiagaraComp->DestroyComponent();
		FVector location = FinishLocation;
		location.Z = OwnerCharacter->GetActorLocation().Z - OwnerCharacter->GetActorUpVector().Z * 66.0f;
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StartEffect, location, GetActorRotation(), FVector(1));
	}
		

	// �� �Ǵ�
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, StartHitRadius, queries, true, ignores, EDrawDebugTrace::None, result, true))
	{
		ACEnemy* target = Cast<ACEnemy>(result.GetActor());
		if (!!target)
		{
			Target = target;
			TargetLocation = Target->GetActorLocation();

			FDamageEvent e;
			Target->TakeDamage(Datas_Attack[0].Power*0.01f, e, OwnerCharacter->GetController(), this);

			// ���� ������ ���� 
			//	=> CustomTimeDilation = 0.0f
			Target->Hold(true);

			return;
		}
	}
}


void ACAction_Cutter::Begin_Action()
{
	Super::Begin_Action();

	if (Target == nullptr)
	{
		End_Action();
		return;
	}


	// �ܰ�(Index)�� �ൿ ����
	if (Index == 0)
	{
		// ī�޶� �׼� ����
		CameraAction(0, Target);

		Timeline.PlayFromStart();
		TargetAnim = Target->GetMesh()->AnimScriptInstance;
		Target->GetMesh()->AnimClass = nullptr;
		Target->GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
	else if (Index == 1)
	{
		// 6���� ��� 0.2�� �������� ����
		UKismetSystemLibrary::K2_SetTimer(this, "SpawnObject", 0.2f, true);
	}
	else if (Index == 2)
	{
		// 6���� ������ 0.2�� �������� ����
		UKismetSystemLibrary::K2_SetTimer(this, "StartLaser", 0.2f, true);
	}
	else if (Index == 3)
	{
	}

	Index++;
}


void ACAction_Cutter::NextAction()
{
	SpawnCount = 0;
	if (Target == nullptr || Index >= (UINT) Datas_Attack.Num())
	{
		End_Action();
		return;
	}

	// �ܰ躰 ��Ÿ�� ���
	if(!!Datas_Attack[Index].AnimMontage)
		PlayMontage_Attack(Index);
	OwnerCharacter->StopAnimMontage(Datas_Attack[Index-1].AnimMontage);


	// ���� �Ⱥ��̰� �ϰ� ProceduralMesh ����
	if (Index == 1)
	{
		Target->Hide(true);

		FTransform transform = Target->GetTransform();
		transform.SetLocation(transform.GetLocation() - Target->GetActorUpVector() * 88);
		ProcMesh = GetWorld()->SpawnActorDeferred<ACProceduralSkeletalMesh>
			(
				ProcClass,
				transform,
				Target,
				nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		UGameplayStatics::FinishSpawningActor(ProcMesh, transform);
		ProcMesh->AttachToActor(Target, FAttachmentTransformRules::KeepWorldTransform);

		for (auto socket : CutSocketName)
		{
			FVector location = Target->GetMesh()->GetSocketLocation(socket);
			TargetSocket.Add(location);		// Ÿ�� ����� �ڸ� �� ���� ��ġ

			FVector start = OwnerCharacter->GetActorLocation();
			FVector direction = (location - start).GetSafeNormal();
			location = OwnerCharacter->GetActorLocation() + direction * StartHitFoward * 4.0f;
			location.Z = OwnerCharacter->GetActorLocation().Z;
			SpawnObjectLocation.Add(location);		// �������� ���� �� ������ ����ϸ� �ڸ��� ƨ�� ��ġ�� ���� ������Ʈ ��ġ
		}

		ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
		if (!!player)
			player->OnBlackOut();

		CameraAction(Index, OwnerCharacter);
	}
	// ���� NextAction�� ���� ī�޶� �׼Ǹ� ����
	else if (Index == 2)
	{
		CameraAction(Index, OwnerCharacter);
	}
	// ������ ��ġ ���� �� �ٽ� ������ ���ư����� ��
	else if (Index == 3)
	{
		FVector start, direction, end;
		for (int32 i = 0 ; i < Lasers.Num() ; i++)
		{
			end = Target->GetMesh()->GetSocketLocation(CutSocketName2[i]);
			start = Lasers[i]->GetActorLocation();
			start.Z = Target->GetActorLocation().Z * 2.0f;
			direction = (end - start).GetSafeNormal();
			//CLog::Print(direction);
			float length = 300 + i * 500;
			Lasers[i]->SetActorLocation(start - direction * length);
			Lasers[i]->ChangeDirection(direction);
		}

		CameraAction(Index, Target);

		Index++;
	}
	// �ܰ� ����
	else if (Index == 4)
	{
		End_Action();
	}

}


void ACAction_Cutter::Abort()
{
	End_Action();
}


void ACAction_Cutter::End_Action()
{
	Super::End_Action();
	OwnerCharacter->StopAnimMontage();


	if (!!Target)
	{
		if (!!EndEffect)
		{
			if (!!NiagaraComp)
				NiagaraComp->DestroyComponent();
			FVector location = FinishLocation;
			location.Z = OwnerCharacter->GetActorLocation().Z - OwnerCharacter->GetActorUpVector().Z * 66.0f;
			NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), EndEffect, location, GetActorRotation(), FVector(1));
		}

		FinishLocation.Z = OwnerCharacter->GetActorLocation().Z;
		Target->SetActorLocation(FinishLocation);
		Target->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(Target->GetActorLocation(), OwnerCharacter->GetActorLocation()));

		Target->Hold(false);
		Target->Hide(false);
		Target->GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
		Target->GetMesh()->AnimScriptInstance = TargetAnim;
		Target->GetMesh()->Play(false);

		FDamageEvent e;
		Target->TakeDamage(Datas_Attack[0].Power, e, OwnerCharacter->GetController(), this);
		Target = nullptr;

	}

	for (int32 i = 0; i < Lasers.Num(); i++)
		Lasers[i]->Hide();

	ActionComp->GetCameraActor()->OnMoveEnd.Remove(CameraHandle);

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	if (!!player)
		player->OffBlackOut();

	for (auto laser : Lasers)
	{
		laser->Hide();
	}

	if (!!ProcMesh)
	{
		ProcMesh->Destroy();
		ProcMesh = nullptr;
	}

	SpawnObjectLocation.Empty();
	TargetSocket.Empty();
}


void ACAction_Cutter::SpawnObject()
{
	CheckFalse(SpawnObjectLocation.Num() > (int32) SpawnCount);

	FTransform transform = OwnerCharacter->GetTransform();
	transform.SetLocation(SpawnObjectLocation[SpawnCount]);
	//CLog::Print(transform.GetLocation());
	ACSpawnObject* skill = GetWorld()->SpawnActorDeferred<ACSpawnObject>
		(
			SpawnObjectClass,
			transform,
			OwnerCharacter,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
	UGameplayStatics::FinishSpawningActor(skill, transform);
	skill->Start(nullptr, false);


	SpawnCount++;
	if (SpawnObjectLocation.Num() <= (int32)SpawnCount)
		UKismetSystemLibrary::K2_ClearTimer(this, "SpawnObject");
}


void ACAction_Cutter::StartLaser()
{
	FVector start = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetActorUpVector();
	FVector direction = (TargetSocket[SpawnCount] - start).GetSafeNormal();
	Lasers[SpawnCount]->Start(start, direction, ProcMesh);

	SpawnCount++;
	if (Lasers.Num() <= (int32)SpawnCount)
		UKismetSystemLibrary::K2_ClearTimer(this, "StartLaser");
}


void ACAction_Cutter::EnemyMoving(float Output)
{
	//CLog::Print(Target->GetActorLocation());
	FVector direction = OwnerCharacter->GetActorForwardVector() * StartHitFoward * 0.5f + OwnerCharacter->GetActorUpVector() * StartHitFoward;
	direction = direction.GetSafeNormal();
	FVector location = TargetLocation + direction * Output * 200;
	location = UKismetMathLibrary::VLerp(TargetLocation, FinishLocation, Output);

	Target->SetActorLocation(location);

	FVector target = OwnerCharacter->GetActorLocation();
	Target->SetActorRotation(UKismetMathLibrary::FindLookAtRotation(location, target));
}





