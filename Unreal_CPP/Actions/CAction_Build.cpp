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
	// �̹� ��ġ ���̾����� ����
	if (bBuildMode)
	{
		BuildReset();
		StateComp->SetIdleMode();
		StatusComp->SetMove();
		ActionComp->Reset();
		return;
	}

	// ��ġ ����
	//	=> �ϴ� ��ġ�� ������Ʈ ����
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

	// �ϴ� ��ġ�� ������Ʈ�� �ִ� ��쿡�� ����
	if (BuildObject)
	{
		// LineTrace�� ��ġ �� ��ġ�� ���� ���θ� üũ
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
		// LineTrace�� �浹 ������ ���� �Ǵ�
		//	=> �浹�� ��(�ٴ�, ��)�� x,y,z�࿡ �󸶳� �����̳Ŀ� ���� ��ġ ���� ���� �Ǵ�
		//	=> ������ ��ġ?, �ٴڿ��� ��ġ? �� �Ǵ�
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