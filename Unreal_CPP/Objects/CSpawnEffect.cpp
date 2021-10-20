#include "CSpawnEffect.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/ShapeComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/CEnemy.h"

ACSpawnEffect::ACSpawnEffect()
{
 	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &NiagaraComp, "NiagaraComp", Scene);
	CHelpers::CreateComponent(this, &Collision, "CollisionComp", Scene);
}

void ACSpawnEffect::Start(float InDamage)
{
	Damage = InDamage;
	UKismetSystemLibrary::K2_SetTimer(this, "OnEnd", Duration, false);	// 일단 최대 시간 설정
	CheckTrue(Speed < 10.0f);

	FVector position = OwnerCharacter->GetActorLocation();

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
}


void ACSpawnEffect::OnEnd()
{
	if (OnEndEvent.IsBound())
	{
		OnEndEvent.Broadcast();
	}
	End();
}

void ACSpawnEffect::End()
{

	bMove = false;
	GetWorld()->DestroyActor(this);
}

void ACSpawnEffect::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACSpawnEffect::OnComponentBeginOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACSpawnEffect::OnComponentEndOverlap);

	OwnerCharacter = Cast<ACharacter>(GetOwner());
	
}

void ACSpawnEffect::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HitTimer += DeltaTime;

	if (HitTimer > HitTime)
	{
		HitTimer = 0.0f;

		for (ACharacter* other : HittedCharacters)
		{
			ACEnemy* enemy = Cast<ACEnemy>(other);
			CheckNull(enemy);
			if (enemy->IsDead())
			{
				HittedCharacters.Remove(other);
			}
			else
			{
				FDamageEvent e = FDamageEvent();
				enemy->TakeDamage(Damage, e, OwnerCharacter->GetController(), OwnerCharacter);	// 주기적으로 데미지를 줌
			}
		}
	}

}

void ACSpawnEffect::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ACharacter* otherCharacter = Cast<ACharacter>(OtherActor);
	CheckNull(otherCharacter);
	CheckTrue(otherCharacter == OwnerCharacter);
	CheckTrue(HittedCharacters.Contains(otherCharacter));

	HittedCharacters.AddUnique(otherCharacter);		// 충돌 대상 저장 (Tick에서 주기적으로 데미지를 줌)
}

void ACSpawnEffect::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ACharacter* otherCharacter = Cast<ACharacter>(OtherActor);
	CheckNull(otherCharacter);
	CheckTrue(otherCharacter == OwnerCharacter);
	CheckFalse(HittedCharacters.Contains(otherCharacter));

	HittedCharacters.Remove(otherCharacter);
}

