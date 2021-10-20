#include "CDestructible.h"
#include "Global.h"
#include "Engine/StaticMesh.h"
#include "Components/StaticMeshComponent.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"


ACDestructible::ACDestructible()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &StaticMeshComp, "StaticMeshComp", Scene);
	CHelpers::CreateComponent(this, &DestructMeshComp, "DestructMeshComp", Scene);
	
	UStaticMesh* staticMesh;
	CHelpers::GetAsset<UStaticMesh>(&staticMesh, "StaticMesh'/Game/StaticMeshes/SM_Rock.SM_Rock'");
	StaticMeshComp->SetStaticMesh(staticMesh);

	StaticMeshComp->OnComponentBeginOverlap.AddDynamic(this, &ACDestructible::OnComponentBeginOverlap);
	StaticMeshComp->OnComponentEndOverlap.AddDynamic(this, &ACDestructible::OnComponentEndOverlap);
	StaticMeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic,ECollisionResponse::ECR_Overlap );
	StaticMeshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	USkeletalMesh* mesh;
	CHelpers::GetAsset<USkeletalMesh>(&mesh, "DestructibleMesh'/Game/StaticMeshes/SM_Rock_DM.SM_Rock_DM'");
	UDestructibleMesh* destructMesh = Cast<UDestructibleMesh>(mesh);
	DestructMeshComp->SetDestructibleMesh(destructMesh);
	
}

void ACDestructible::BeginPlay()
{
	Super::BeginPlay();
	DestructMeshComp->SetVisibility(false);
	DestructMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACDestructible::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (DestructTime > 0.0f)
	{
		DestructTime -= DeltaTime;

		if (DestructTime < 0.0f)
			GetWorld()->DestroyActor(this);
	}
}


void ACDestructible::Initialize(float InSize)
{
	Size = InSize;
	FVector size = FVector(Size);
	size.Y *= 1.25f;
	StaticMeshComp->SetWorldScale3D(size);
	DestructMeshComp->SetWorldScale3D(size);
}

void ACDestructible::Destruct(FVector InDirection, float Power)
{
	CheckNull(DestructMeshComp);
	CheckNull(StaticMeshComp);

	DestructMeshComp->SetVisibility(true);
	DestructMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	DestructMeshComp->SetNotifyRigidBodyCollision(true);
	DestructMeshComp->LargeChunkThreshold = 100.0f;

	StaticMeshComp->SetVisibility(false);
	StaticMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	DestructMeshComp->ApplyDamage(Power * Size, GetActorLocation(), InDirection, 1.0f);
	DestructTime = 3.0f;
}

void ACDestructible::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	DestructCount -= 1;
	if (DestructCount < 0)
	{
		FVector direction = (GetActorLocation() - OtherActor->GetActorLocation()).GetSafeNormal();
		Destruct(direction);
	}
}

void ACDestructible::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}