#include "CAction_Build.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Objects/CBuildObject.h"
#include "Character/CPlayer.h"


ACAction_Build::ACAction_Build()
	:ACAction()
{

}


void ACAction_Build::BeginPlay()
{
	Super::BeginPlay();

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Build::AbortByTypeChange);
}


void ACAction_Build::Action()
{
	// 이미 설치 중이었으면 종료
	if (bBuildMode)
	{
		BuildReset();
		StateComp->SetIdleMode();
		StatusComp->SetMove();
		ActionComp->Reset();
		return;
	}

	// 설치 시작
	//	=> 일단 설치할 오브젝트 스폰
	BuildSpawn();
}


void ACAction_Build::Begin_Action()
{
	CheckFalse(StateComp->IsIdleMode());
	CheckFalse(bBuildMode);
	CheckFalse(CanBuild);
	CheckNull(BuildObject);

	BuildObject->BuildStart(OwnerCharacter);
	StateComp->SetActionMode();

	PlayMontage_Common(0);
}


void ACAction_Build::End_Action()
{
	Super::End_Action();

	CheckNull(BuildObject);
	
	BuildObject->BuildFinish();
	bBuildMode = false;
	CanBuild = true;
}


void ACAction_Build::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	BuildTick();

}


void ACAction_Build::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Build && InPrevType != InNewType)
	{
		End_Action();
		BuildReset();
	}
}


void ACAction_Build::Abort()
{
	End_Action();
	BuildReset();
}


void ACAction_Build::BuildSpawn()
{
	BuildObject = Cast<ACBuildObject>(OwnerCharacter->GetWorld()->SpawnActor(Datas_Common[0].BuildObjectClass));
	BuildObject->BuildSpawn();
	bBuildMode = true;
}


void ACAction_Build::BuildReset()
{
	bBuildMode = false;
	CanBuild = true;

	if (BuildObject)
	{
		OwnerCharacter->GetWorld()->DestroyActor(BuildObject);
		BuildObject = nullptr;
	}

	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	if (!!player)
		player->SetDilationTime(1.0f);
}


void ACAction_Build::BuildTick()
{
	CheckFalse(bBuildMode);

	// 일단 설치할 오브젝트가 있는 경우에만 실행
	if (BuildObject)
	{
		// LineTrace로 설치 될 위치와 가능 여부를 체크
		BuildTrace(BuildLocation, CanBuild);
		BuildObject->BuildCheck(CanBuild);
		BuildObject->SetActorLocation(BuildLocation);


		FRotator rotation = OwnerCharacter->GetController()->GetControlRotation();
		if (bRotation == false)
		{
			rotation.Roll = 0.0f;
			rotation.Pitch = 0.0f;
		}
		BuildObject->SetActorRotation(rotation);

	}
	else
		BuildReset();
}


void ACAction_Build::BuildTrace(FVector& OutLocation, bool& OutBuildAble)
{
	OutBuildAble = false;

	FVector start = OwnerCharacter->GetActorLocation();
	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	CheckNull(player);
	FVector end = start + player->CameraDirection() * Datas_Common[0].BuildRange;

	FCollisionQueryParams params;
	params.AddIgnoredActor(this);
	params.AddIgnoredActor(OwnerCharacter);

	FHitResult hitResult;
	if (GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility, params))
	{
		// LineTrace의 충돌 지점의 방향 판단
		//	=> 충돌한 면(바닥, 벽)이 x,y,z축에 얼마나 수직이냐에 따라 설치 가능 여부 판단
		//	=> 벽에만 설치?, 바닥에만 설치? 를 판단
		if (Datas_Common[0].BuildDotX == true &&
			FMath::Abs(UKismetMathLibrary::Dot_VectorVector(hitResult.Normal, FVector(1, 0, 0))) > Datas_Common[0].BuildDot)
				OutBuildAble = true;

		if (Datas_Common[0].BuildDotY == true &&
			FMath::Abs(UKismetMathLibrary::Dot_VectorVector(hitResult.Normal, FVector(0, 1, 0))) > Datas_Common[0].BuildDot)
				OutBuildAble = true;

		if (Datas_Common[0].BuildDotZ == true &&
			FMath::Abs(UKismetMathLibrary::Dot_VectorVector(hitResult.Normal, FVector(0, 0, 1))) > Datas_Common[0].BuildDot)
				OutBuildAble = true;

		OutLocation = hitResult.ImpactPoint;
	}
	else
		OutLocation = hitResult.TraceEnd;
}