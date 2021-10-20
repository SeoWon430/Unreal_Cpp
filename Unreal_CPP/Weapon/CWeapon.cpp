#include "CWeapon.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "Components/ShapeComponent.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/CapsuleComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Objects/CSpawnObject.h"
#include "Components/CProceduralSliceComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProceduralMeshComponent.h"

ACWeapon::ACWeapon()
{
 	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USceneComponent>(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &StaticMeshComp, "StaticMeshComp", Scene);
	CHelpers::CreateComponent(this, &Collision, "CollisionComp", StaticMeshComp);
	CHelpers::CreateComponent(this, &NiagaraComp, "NiagaraComp", Scene);

	CHelpers::CreateActorComponent(this, &SliceComp, "SliceComp");

	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	NiagaraComp->SetVisibility(false);

}


void ACWeapon::BeginPlay()
{
	OwnerCharacter = Cast<ACharacter>(GetOwner());
	State = CHelpers::GetComponent<UCStateComponent>(OwnerCharacter);
	Status = CHelpers::GetComponent<UCStatusComponent>(OwnerCharacter);

	Collision->OnComponentBeginOverlap.AddDynamic(this, &ACWeapon::OnComponentBeginOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(this, &ACWeapon::OnComponentEndOverlap);
	

	OffCollision();

	Super::BeginPlay();
	
}

// Called every frame
void ACWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACWeapon::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(OwnerCharacter == OtherActor);
	CheckTrue(OwnerCharacter->GetClass() == OtherActor->GetClass());


	ACSpawnObject* object = Cast<ACSpawnObject>(OtherActor);
	if (!!object) {
		FDamageEvent e;
		object->TakeDamage(1000, e, OwnerCharacter->GetController(), OwnerCharacter);
	}

	//SliceComp->Slice(SweepResult, GetActorRightVector());


	
	
	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	queries.Add(EObjectTypeQuery::ObjectTypeQuery2);
	TArray<AActor*> ignores;
	ignores.Add(this);
	ignores.Add(OwnerCharacter);
	FVector start = GetActorLocation();
	FVector end = start + GetActorForwardVector()*300.0f;
	FHitResult result;

	UActorComponent* comp = OtherActor->GetComponentByClass(UProceduralMeshComponent::StaticClass());
	if (!!comp)
	{
		if (UKismetSystemLibrary::LineTraceSingleByProfile(GetWorld(), start, end, "Visibility", true, ignores, EDrawDebugTrace::ForOneFrame, result, true, FLinearColor::Red, FLinearColor::Red, 50))
			//if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, result, true, FLinearColor::Red, FLinearColor::Red, 50))
		{
			//CLog::Print(bFromSweep);
			//CLog::Print(SweepResult.ImpactPoint);
			SliceComp->Slice(result, GetActorRightVector());
		}
	}
		



	if (OnAttachmentBeginOverlap.IsBound())
		OnAttachmentBeginOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
}

void ACWeapon::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OnAttachmentEndOverlap.IsBound())
		OnAttachmentEndOverlap.Broadcast(OwnerCharacter, this, Cast<ACharacter>(OtherActor));
}

void ACWeapon::OnCollision()
{
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}

void ACWeapon::OffCollision()
{
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ACWeapon::AttachTo(FName InSocketName)
{
	/*
	if (InSocketName.IsEqual("Hide"))
	{
		TArray<UStaticMeshComponent*> mesh;
		GetComponents<UStaticMeshComponent>(mesh);
		for (UStaticMeshComponent* m : mesh)
			m->SetVisibility(false);
	}
	else
	{
		TArray<UStaticMeshComponent*> mesh;
		GetComponents<UStaticMeshComponent>(mesh);
		for (UStaticMeshComponent* m : mesh)
			m->SetVisibility(true);
	}*/

	if (InSocketName.IsNone() || InSocketName.IsEqual(""))
	{
		InSocketName = "Weapon_Holster";
	}


	AttachToComponent
	(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::KeepWorld, true),
		InSocketName
	);
	SetActorRelativeLocation(FVector(0));
	SetActorRelativeRotation(FRotator(0));
}

void ACWeapon::AttachToCollision(USceneComponent* InComponent, FName InSocketName)
{
	InComponent->AttachToComponent
	(
		OwnerCharacter->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::KeepRelative, true),
		InSocketName
	);
	SetActorRelativeLocation(FVector(0));
	SetActorRelativeRotation(FRotator(0));
	ActionEnd();
}
