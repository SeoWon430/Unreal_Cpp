#include "CThrowObject.h"
#include "Global.h"
#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/Character.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/ProjectileMovementComponent.h"


ACThrowObject::ACThrowObject()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &Collision, "CollisionComp", Scene);
	CHelpers::CreateComponent(this, &StaticMeshComp, "StaticMeshComp", Scene);

	CHelpers::CreateActorComponent(this, &ProjectileMovement, "ProjectileMovement");
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ACThrowObject::OnConstruction(const FTransform& Transform)
{
	Collision->SetSphereRadius(Range);

}

void ACThrowObject::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACThrowObject::OnComponentBeginOverlap);
}


void ACThrowObject::StartThrow(ACharacter* InOwnerCharacter, float InPower, FVector InDestination, FRotator InDirection)
{
	bMove = true;
	OwnerCharacter = InOwnerCharacter;
	ProjectileMovement->InitialSpeed = InPower;
	Destination = InDestination;
	SetActorRotation(InDirection);
	PrevLocation = GetActorLocation();

	// 안하면 너무 느리게 날아감
	//	(빠르게 날아가게 속도를 높이면 곡선을 안그리고 직선으로 날아가기 때문)
	CustomTimeDilation = 3.0f;
}

void ACThrowObject::Tick(float DeltaTime)
{
	CheckFalse(bMove);
	Super::Tick(DeltaTime);

	if (OwnerCharacter == nullptr)
		GetWorld()->DestroyActor(this);

	// 날아가야 할 최종지점(Destination)에서 강제로 Impact 호출
	//	거리로 판단했을 때, 속도가 빠르면 프레임이 무시 될수 있음
	//	=> Destination을 기준으로 이전과 현재 위치간 방향벡터 내적으로 다른 방향이면 Impact 호출
	FVector dir1 = (PrevLocation - Destination).GetSafeNormal();
	FVector dir2 = (GetActorLocation() - Destination).GetSafeNormal();
	float dot = FVector::DotProduct(dir1, dir2);
	
	PrevLocation = GetActorLocation();

	if (dot < 0.9f)
		Impact();

}

void ACThrowObject::OnComponentBeginOverlap(UPrimitiveComponent * OverlappedComponent, AActor * OtherActor, UPrimitiveComponent * OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	CheckNull(OwnerCharacter)
	ACharacter* other = Cast<ACharacter>(OtherActor);
	if (!!other && other == OwnerCharacter)
	{
		return;
	}
	Impact();
}


void ACThrowObject::Impact()
{
	CheckFalse(bMove);
	bMove = false;

	StaticMeshComp->SetVisibility(false);

	if (!!Particle)
		NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Particle, GetActorLocation(), GetActorRotation());

	ProjectileMovement->ProjectileGravityScale = 0.0f;
	ProjectileMovement->Velocity = FVector(0);
	ProjectileMovement->StopMovementImmediately();


	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	queries.Add(EObjectTypeQuery::ObjectTypeQuery3);
	TArray<AActor*> ignores;
	ignores.Add(this);
	ignores.Add(OwnerCharacter);

	TArray<FHitResult> hitResults;

	if (UKismetSystemLibrary::SphereTraceMultiForObjects
		(GetWorld(), GetActorLocation(), GetActorLocation() + GetActorUpVector()
		, DamageRange, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResults, true))
	{
		for (FHitResult hitResult : hitResults)
		{
			if (!!hitResult.GetActor())
			{
				ACharacter* character = Cast<ACharacter>(hitResult.GetActor());
				if (!!character)
				{
					HittedCharacters.AddUnique(character);
				}
			}
		}
	}

	

	for (ACharacter* character : HittedCharacters)
	{
		FDamageEvent e;
		character->TakeDamage(Damage, e, character->GetController(), this);
	}

	UKismetSystemLibrary::K2_SetTimer(this, "End", 5.0f, false);
}



void ACThrowObject::End()
{
	GetWorld()->DestroyActor(this);
}
