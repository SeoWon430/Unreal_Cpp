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

	// ī�޶� ����
	TimelineFloat.BindUFunction(this, "Zooming");
	Timeline.AddInterpFloat(Curve, TimelineFloat);
	Timeline.SetPlayRate(100);


	// ������ ���� ��θ� �׷��ֱ� ���� Niagara
	//	=> �ش� Niagara�� �Ķ���ͷ� ��ġ������ �־� �� ���̻��̿� Trail�� �׸����� ��
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


	// ������ ���� ��ο��� ���� ������ ǥ���� ����
	//	=> DestinationEffect�� ���� ���ͷ� ������ ���¿����� ������
	DestinationEffect->SetVisible();

	CheckFalse(StateComp->IsIdleMode());

	PlayMontage_Common(0);

	// �þ� ����
	OnAim();
}


void ACAction_Throw::Begin_Action()
{
	CheckFalse(bThrow);

	FVector location = OwnerCharacter->GetMesh()->GetSocketLocation("Hand_ThrowItem");
	FRotator rotator = OwnerCharacter->GetController()->GetControlRotation();

	// ������ ������Ʈ�� ����
	//	=> �ش� ������Ʈ�� ProjectileComponent�� ���� ���ư� ��
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

	// ������Ʈ ������
	throwObject->StartThrow(OwnerCharacter, ThrowPower, Destination, rot);

	UGameplayStatics::FinishSpawningActor(throwObject, transform);

	// ������ ���� ������ ǥ���� ���� �Ⱥ��̰� �ϱ�
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

	// ������ ���� ��θ� �׸� Niagara�� Trail �ȱ׸���
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

	// ������ �˵� ������ ���� ���� ���
	FVector direction = player->CameraDirection();
	direction *= 0.5f;
	direction.Z += 0.3f;
	direction.Z *= 2.0f;
	direction = direction.GetSafeNormal() * ThrowPower;

	// PredictProjectilePath�� ProjectileComponent�� ��� �������� ���� ��θ� VertexArray�� ��������
	//	=> ���� ���� ���� �������� ó���� ��쿡�� ���� ��ο� ���� ���ư��� �˵��� ��ġ��
	if (UGameplayStatics::PredictProjectilePath(GetWorld(), hitResult, VertexArray, destination, start, direction, true, 20, queries, false, ignores, EDrawDebugTrace::None, 0.0f, 5.0f, 5.0f))
	{
		// ������ ���� ������ ���� ���͸� ��ġ�� ������
		Destination = hitResult.ImpactPoint;
		DestinationEffect->SetDestination(hitResult.ImpactPoint, hitResult.ImpactNormal);
	}
	else
	{
		Destination = VertexArray[VertexArray.Num()-1];
		DestinationEffect->SetDestination(destination, FVector(0, 0, 1));
	}
	// *****
	//	����� ���̸� ������ ������ �������� �ǵ��� ���� ������ �����ϸ�, ������ ���Ͱ� ���� ���ư��� �ӵ��� �ʹ� ����
	//	�ݴ��, �ӵ��� ������ �ҷ��� �������� ���� ������ �ǹ���
	//		=> ������ �������� �ǵ��� �ϸ� ���� ���ư� ������ �ӵ��� ������ ������, �� ������ CustomTimeDilation ���� �ø�
	//		=> CustomTimeDilation = 3.0f;	// �ش� ���͸� �ӵ��� 3�� ������ ���� ��


	// ������ ���� ��� �׸���
	//	=> Niagara �Ķ���Ϳ� ��ġ���� �־� �� ���̿� Trail�� �׸����� ��
	//	=> ���� ��ġ������ �� PredictProjectilePath���� ����� VertexArray ������
	for (int32 i = 0; i < Trail.Num(); i++)
	{
		if (i < VertexArray.Num()-2)
		{
			Trail[i]->SetFloatParameter("LaserSpawnProb", 1);				// �׸� ����Ʈ �� : 1
			Trail[i]->SetVectorParameter("LaserStart", VertexArray[i]);		// ���� ��ġ
			Trail[i]->SetVectorParameter("LaserEnd", VertexArray[i + 1]);	// �� ��ġ
		}
		else
		{	
			Trail[i]->SetFloatParameter("LaserSpawnProb", 0);		// ����Ʈ �ȱ׸�
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