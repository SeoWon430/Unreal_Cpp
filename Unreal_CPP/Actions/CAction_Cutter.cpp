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

	// 6개의 레이저 미리 생성
	//	=> 자를 소캣을 Blueprint에서 6개로 지정함
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


// SphereTrace로 적이 타격 되면 다음 단계 실행
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

	// 시작 이펙트 스폰
	if (!!StartEffect)
	{
		if(!!NiagaraComp)
			NiagaraComp->DestroyComponent();
		FVector location = FinishLocation;
		location.Z = OwnerCharacter->GetActorLocation().Z - OwnerCharacter->GetActorUpVector().Z * 66.0f;
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), StartEffect, location, GetActorRotation(), FVector(1));
	}
		

	// 적 판단
	if (UKismetSystemLibrary::SphereTraceSingleForObjects(GetWorld(), start, end, StartHitRadius, queries, true, ignores, EDrawDebugTrace::None, result, true))
	{
		ACEnemy* target = Cast<ACEnemy>(result.GetActor());
		if (!!target)
		{
			Target = target;
			TargetLocation = Target->GetActorLocation();

			FDamageEvent e;
			Target->TakeDamage(Datas_Attack[0].Power*0.01f, e, OwnerCharacter->GetController(), this);

			// 적의 움직임 봉쇄 
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


	// 단계(Index)별 행동 실행
	if (Index == 0)
	{
		// 카메라 액션 실행
		CameraAction(0, Target);

		Timeline.PlayFromStart();
		TargetAnim = Target->GetMesh()->AnimScriptInstance;
		Target->GetMesh()->AnimClass = nullptr;
		Target->GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}
	else if (Index == 1)
	{
		// 6개의 기둥 0.2초 간격으로 생성
		UKismetSystemLibrary::K2_SetTimer(this, "SpawnObject", 0.2f, true);
	}
	else if (Index == 2)
	{
		// 6개의 레이져 0.2초 간격으로 생성
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

	// 단계별 몽타쥬 재생
	if(!!Datas_Attack[Index].AnimMontage)
		PlayMontage_Attack(Index);
	OwnerCharacter->StopAnimMontage(Datas_Attack[Index-1].AnimMontage);


	// 적을 안보이게 하고 ProceduralMesh 생성
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
			TargetSocket.Add(location);		// 타격 대상의 자를 본 소켓 위치

			FVector start = OwnerCharacter->GetActorLocation();
			FVector direction = (location - start).GetSafeNormal();
			location = OwnerCharacter->GetActorLocation() + direction * StartHitFoward * 4.0f;
			location.Z = OwnerCharacter->GetActorLocation().Z;
			SpawnObjectLocation.Add(location);		// 레이저로 위의 본 소켓을 통과하며 자르고 튕길 위치에 세울 오브젝트 위치
		}

		ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
		if (!!player)
			player->OnBlackOut();

		CameraAction(Index, OwnerCharacter);
	}
	// 다음 NextAction을 위해 카메라 액션만 실행
	else if (Index == 2)
	{
		CameraAction(Index, OwnerCharacter);
	}
	// 레이저 위치 조정 후 다시 적에게 날아가도록 함
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
	// 단계 종료
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





