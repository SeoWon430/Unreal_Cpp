#include "CAction_Throw.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Objects/CThrowDestination.h"
#include "Objects/CThrowObject.h"
#include "Character/CPlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"


ACAction_Throw::ACAction_Throw()
	:ACAction()
{
	CHelpers::CreateComponent(this, &Scene, "Scene");

	CHelpers::GetAsset<UCurveFloat>(&Curve, "CurveFloat'/Game/Actions/Curve_Aim.Curve_Aim'");
	CHelpers::GetClass<ACThrowObject>(&ThrowObject, "Blueprint'/Game/Objects/Trajectory/BP_CThrowObject.BP_CThrowObject_C'");

	CHelpers::GetAsset<UNiagaraSystem>(&TrailNiagara, "NiagaraSystem'/Game/Niagara/Laser/NS_Trail.NS_Trail'");
}


void ACAction_Throw::BeginPlay()
{
	Super::BeginPlay();


	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Throw::AbortByTypeChange);


	FTransform transform;
	transform.SetLocation(OwnerCharacter->GetActorLocation());
	DestinationEffect = GetWorld()->SpawnActorDeferred<ACThrowDestination>
		(
			ACThrowDestination::StaticClass(),
			transform,
			OwnerCharacter,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
	UGameplayStatics::FinishSpawningActor(DestinationEffect, transform);
	DestinationEffect->SetHide();
	DestinationEffect->SetActorRelativeScale3D(FVector(0.5f));

	SpringArm = CHelpers::GetComponent<USpringArmComponent>(OwnerCharacter);

	// 카메라 줌인
	TimelineFloat.BindUFunction(this, "Zooming");
	Timeline.AddInterpFloat(Curve, TimelineFloat);
	Timeline.SetPlayRate(100);


	// 던지기 예상 경로를 그려주기 위한 Niagara
	//	=> 해당 Niagara의 파라미터로 위치값들을 넣어 그 사이사이에 Trail을 그리도록 함
	for (int32 i = 0; i < TrailCount; i++)
	{
		UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAttached
		(
			TrailNiagara, Scene, FName()
			, GetActorLocation(), FRotator()
			, EAttachLocation::KeepRelativeOffset, false);
		if(!!niagara)
			Trail.Add(niagara);
	}
}


void ACAction_Throw::Action()
{
	if (bThrow)
	{
		DestinationEffect->SetHide();
		OwnerCharacter->StopAnimMontage(Datas_Common[0].AnimMontage);
		End_Action();
		return;
	}
	bThrow = true;


	// 던지기 예상 경로에서 최종 지점을 표시할 액터
	//	=> DestinationEffect는 구형 액터로 던지기 상태에서만 렌더링
	DestinationEffect->SetVisible();

	CheckFalse(StateComp->IsIdleMode());

	PlayMontage_Common(0);

	// 시야 조정
	OnAim();
}


void ACAction_Throw::Begin_Action()
{
	CheckFalse(bThrow);

	FVector location = OwnerCharacter->GetMesh()->GetSocketLocation("Hand_ThrowItem");
	FRotator rotator = OwnerCharacter->GetController()->GetControlRotation();

	// 실제로 오브젝트를 던짐
	//	=> 해당 오브젝트는 ProjectileComponent에 의해 날아갈 것
	FTransform transform;
	transform.SetLocation(OwnerCharacter->GetActorLocation());
	ACThrowObject* throwObject = GetWorld()->SpawnActorDeferred<ACThrowObject>
		(
			ThrowObject,
			transform,
			OwnerCharacter,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);

	FRotator rot = UKismetMathLibrary::FindLookAtRotation(VertexArray[0], VertexArray[1]);

	// 오브젝트 던지기
	throwObject->StartThrow(OwnerCharacter, ThrowPower, Destination, rot);

	UGameplayStatics::FinishSpawningActor(throwObject, transform);

	// 던지기 최종 지점을 표시할 액터 안보이게 하기
	DestinationEffect->SetHide();

	OwnerCharacter->StopAnimMontage(Datas_Common[0].AnimMontage);

	PlayMontage_Common(1);
}


void ACAction_Throw::End_Action()
{
	DestinationEffect->SetHide();
	StateComp->SetIdleMode();
	StatusComp->SetMove();
	ActionComp->Reset();


	OffAim();
	bThrow = false;

	// 던지기 예상 경로를 그린 Niagara의 Trail 안그리기
	for (int32 i = 0; i < Trail.Num(); i++)
	{
		Trail[i]->SetFloatParameter("LaserSpawnProb", 0);
	}
}


void ACAction_Throw::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Timeline.TickTimeline(DeltaTime);

	CheckFalse(bThrow)
	ACPlayer* player = Cast<ACPlayer>(OwnerCharacter);
	CheckNull(player)

	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ignores;
	ignores.Add(this);

	FHitResult hitResult;
	FVector destination;
	FVector start = OwnerCharacter->GetActorLocation();

	// 포물선 궤도 조정을 위한 방향 계산
	FVector direction = player->CameraDirection();
	direction *= 0.5f;
	direction.Z += 0.3f;
	direction.Z *= 2.0f;
	direction = direction.GetSafeNormal() * ThrowPower;

	// PredictProjectilePath은 ProjectileComponent를 사용 했을때의 예상 경로를 VertexArray에 리턴해줌
	//	=> 같은 힘과 같은 방향으로 처리할 경우에만 예상 경로와 실제 날아가는 궤도가 일치함
	if (UGameplayStatics::PredictProjectilePath(GetWorld(), hitResult, VertexArray, destination, start, direction, true, 20, queries, false, ignores, EDrawDebugTrace::None, 0.0f, 5.0f, 5.0f))
	{
		// 예상경로 최종 지점에 구형 액터를 위치켜 보여줌
		Destination = hitResult.ImpactPoint;
		DestinationEffect->SetDestination(hitResult.ImpactPoint, hitResult.ImpactNormal);
	}
	else
	{
		Destination = VertexArray[VertexArray.Num()-1];
		DestinationEffect->SetDestination(destination, FVector(0, 0, 1));
	}
	// *****
	//	충분한 높이를 가지는 볼록한 포물선이 되도록 힘과 방향을 조절하면, 던져진 액터가 실제 날아갈때 속도가 너무 느림
	//	반대로, 속도를 빠르게 할려면 포물선이 거의 수평이 되버림
	//		=> 볼록한 포물선이 되도록 하면 실제 날아갈 액터의 속도가 느리기 때문에, 이 액터의 CustomTimeDilation 값을 올림
	//		=> CustomTimeDilation = 3.0f;	// 해당 액터만 속도가 3배 빠르게 진행 됨


	// 던지기 예상 경로 그리기
	//	=> Niagara 파라미터에 위치값을 넣어 그 사이에 Trail을 그리도록 함
	//	=> 넣을 위치값들은 위 PredictProjectilePath에서 얻어진 VertexArray 값들임
	for (int32 i = 0; i < Trail.Num(); i++)
	{
		if (i < VertexArray.Num()-2)
		{
			Trail[i]->SetFloatParameter("LaserSpawnProb", 1);				// 그릴 이펙트 수 : 1
			Trail[i]->SetVectorParameter("LaserStart", VertexArray[i]);		// 시작 위치
			Trail[i]->SetVectorParameter("LaserEnd", VertexArray[i + 1]);	// 끝 위치
		}
		else
		{	
			Trail[i]->SetFloatParameter("LaserSpawnProb", 0);		// 이펙트 안그림
		}
	}
}


void ACAction_Throw::OnThrowBeginOverlap(FHitResult InHitResult)
{

}


void ACAction_Throw::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Rest && InPrevType != InNewType)
		End_Action();
}


void ACAction_Throw::Abort()
{
	End_Action();
}


void ACAction_Throw::OnAim()
{
	CheckNull(SpringArm)
	CheckTrue(bThrow)

	SpringArm->bEnableCameraLag = false;

	Timeline.PlayFromStart();
}


void ACAction_Throw::OffAim()
{
	CheckNull(SpringArm)
	CheckFalse(bThrow)

	SpringArm->bEnableCameraLag = true;

	Timeline.ReverseFromEnd();
}


void ACAction_Throw::Zooming(float Output)
{
	SpringArm->TargetArmLength = Output;
}