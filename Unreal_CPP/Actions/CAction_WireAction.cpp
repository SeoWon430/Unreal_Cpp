#include "CAction_WireAction.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/CActionComponent.h"
#include "Objects/CBuildObject.h"
#include "Objects/CBuildProjectile.h"
#include "Character/CPlayer.h"


void ACAction_WireAction::BeginPlay()
{
	Super::BeginPlay();

	bRotation = true;

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_WireAction::AbortByTypeChange);
}


void ACAction_WireAction::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Wire && InPrevType != InNewType)
		End_Action();
}


void ACAction_WireAction::Abort()
{
	End_Action();
}


void ACAction_WireAction::Action()
{
	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	CheckNull(player);

	// 스킬 첫 입력시 오브젝트 설치
	if (Target == nullptr)
	{
		player->SetDilationTime(0.25f);
		Super::Action();
		return;
	}

	player->SetDilationTime(1.0f);

	// 스킬 두번째 입력시 설치한 방향으로 날아감
	bStartWireAction = true;
	FVector direction = OwnerCharacter->GetActorLocation() - Target->GetActorLocation();
	TargetLength = direction.Size();
	Speed = FMath::Clamp(TargetLength / 1000, 1.0f, 10.0f);

	MoveRightVector = UKismetMathLibrary::Cross_VectorVector(direction, FVector(0, 0, 1)).GetSafeNormal();
	MoveRightVector.Z = FMath::Abs(MoveRightVector.Z) + 1;

	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0);

	PlayMontage_Common(1);

	// 날아가는 중에는 벽타기 막음 (날아가는것이 종료되면 바로 벽을 타게 할 것)
	player->SetWallCheckOff();
	player->CollisionOff();

}


void ACAction_WireAction::Begin_Action()
{
	// 좌클릭시 실행
	//Super::Begin_Action();
	CheckFalse(StateComp->IsIdleMode());
	CheckFalse(bBuildMode);
	CheckFalse(CanBuild);
	CheckNull(BuildObject);

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	CheckNull(player);
	player->SetDilationTime(1.0f);

	bBuildMode = false;
	FVector location = OwnerCharacter->GetMesh()->GetSocketLocation(Datas_Common[0].SpawnSocket);
	BuildObject->BuildStart(OwnerCharacter);
	BuildObject->SetActorRotation(OwnerCharacter->GetController()->GetControlRotation());

	ACBuildProjectile* projectile = Cast<ACBuildProjectile>(BuildObject);
	if (!!projectile)
		projectile->SetDirection(location, BuildLocation, Datas_Common[0].SpawnSocket);

	if (player->IsWallOn())
	{
		player->OnJump();
		End_Action();
	}
	else
	{
		StateComp->SetActionMode();
		PlayMontage_Common(0);
	}
}


void ACAction_WireAction::End_Action()
{
	if (Target == nullptr)
	{
		Super::End_Action();

		AActor* buildObjece = BuildObject;
		if (!!buildObjece)
			Target = buildObjece;
	}

}

void ACAction_WireAction::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckNull(Target);
	CheckFalse(bStartWireAction);

	ActionWire(DeltaTime);
	

}

void ACAction_WireAction::ActionWire(float DeltaTime)
{
	FVector owner = OwnerCharacter->GetActorLocation();
	FVector target = Target->GetActorLocation();

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	CheckNull(player);
	UCStateComponent* state = CHelpers::GetComponent<UCStateComponent>(player);
	UCStatusComponent* status = CHelpers::GetComponent<UCStatusComponent>(player);
	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(player);

	float currentTargetLength = (owner - target).Size();

	// 목표 지점에 도착
	if (currentTargetLength < 50)
	{
		bStartWireAction = false;
		GetWorld()->DestroyActor(Target);
		GetWorld()->DestroyActor(BuildObject);
		Target = nullptr;
		BuildObject = nullptr;

		player->GetCharacterMovement()->Velocity = FVector(0);
		player->CollisionOn();
		player->SetWallCheckOn();
		player->SetCanWallOn();
		state->SetIdleMode();
		status->SetMove();
		action->Reset();

		OwnerCharacter->StopAnimMontage();
		return;
	}


	// 목표 지점으로 이동
	{
		state->SetActionMode();
		status->SetStop();

		FVector direction = (target - owner).GetSafeNormal();
		float rate = currentTargetLength / TargetLength;
		// sin을 통해 곡선을 그리게 움직임
		FVector offset = MoveRightVector * UKismetMathLibrary::Sin(rate * UKismetMathLibrary::GetPI()) * 500 * Speed * DeltaTime;
		offset.Z *= 1.5f;
		FVector location = owner + direction * 2000 * Speed * DeltaTime;
		OwnerCharacter->SetActorLocation(location + offset);

		// 최종 지점은 벽이기에 캐릭터를 이에 맞게 회전
		FRotator rotation = UKismetMathLibrary::FindLookAtRotation(owner, target);
		rotation.Pitch = 0;
		rotation.Roll = 0;
		OwnerCharacter->SetActorRotation(rotation);
	}

}
