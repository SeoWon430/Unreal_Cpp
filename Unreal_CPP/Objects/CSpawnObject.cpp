#include "CSpawnObject.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"

ACSpawnObject::ACSpawnObject()
{
 	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &StaticMeshComp, "StaticMeshComp", Scene);
	CHelpers::CreateComponent(this, &Collision, "CollisionComp", StaticMeshComp);
}

void ACSpawnObject::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACSpawnObject::OnComponentBeginOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACSpawnObject::OnComponentEndOverlap);

	OwnerCharacter = Cast<ACharacter>(GetOwner());
}

void ACSpawnObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckFalse(bMove);
	CheckTrue(Speed < 10.0f);

	FVector position = GetActorLocation();
	
	// 이동 완료시 이펙트 발생
	if (position.Z < Destination.Z)
	{
		if (!!Effect)
		{
			EffectTransform.AddToTranslation(GetActorLocation());
			EffectTransform.TransformRotation(GetActorRotation().Quaternion());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Effect, EffectTransform);
		}
		if (!!Effect2)
		{
			NiagaraComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), Effect2, GetActorLocation(), GetActorRotation(), EffectTransform.GetScale3D());

		}

		bMove = false;
	}

	// 설정 위치로 이동
	position += Direction* Speed * DeltaTime;
	SetActorLocation(position);
}


void ACSpawnObject::Start(AActor* InLookActor, bool IsOwnerLocation)
{
	// 이 오브젝트의 유지 시간 설정
	UKismetSystemLibrary::K2_SetTimer(this, "End", Duration, false);
	CheckTrue(Speed < 10.0f);

	// 날아갈 방향 설정
	FVector position = GetActorLocation();
	if(IsOwnerLocation == true)
		position= OwnerCharacter->GetActorLocation();

	FVector start = position;
	start += OwnerCharacter->GetActorForwardVector() * StartPosition.X;
	start += OwnerCharacter->GetActorRightVector() * StartPosition.Y;
	start += OwnerCharacter->GetActorUpVector() * StartPosition.Z;


	Destination = position + FVector(0, 0, -90);
	Destination += OwnerCharacter->GetActorForwardVector() * DestinationPosition.X;
	Destination += OwnerCharacter->GetActorRightVector() * DestinationPosition.Y;
	Destination += OwnerCharacter->GetActorUpVector() * DestinationPosition.Z;

	Direction = (Destination - start).GetSafeNormal();

	bMove = true;
	SetActorLocation(start);

	if(!!InLookActor)
		SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), InLookActor->GetActorLocation()));
}


void ACSpawnObject::End()
{
	bMove = false;
	GetWorld()->DestroyActor(this);
}

float ACSpawnObject::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return 0;
}

void ACSpawnObject::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());


	if (OnAttachmentBeginOverlap.IsBound())
		OnAttachmentBeginOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
		
}

void ACSpawnObject::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OnAttachmentEndOverlap.IsBound())
		OnAttachmentEndOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
}