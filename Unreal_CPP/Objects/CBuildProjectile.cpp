#include "CBuildProjectile.h"
#include "Global.h"
#include "CableComponent.h"
#include "GameFramework/Character.h"

ACBuildProjectile::ACBuildProjectile()
	:ACBuildObject()
{
	PrimaryActorTick.bCanEverTick = true;


	UStaticMesh* mesh;
	CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/Objects/SM_Cone.SM_Cone'");
	MeshComp->SetStaticMesh(mesh);
	MeshComp->SetRelativeLocation(FVector(0, 0, 0));
	MeshComp->SetRelativeRotation(FRotator(-90, 0, 0));
	MeshComp->SetRelativeScale3D(FVector(0.2f, 0.2f, 1.0f));

	CHelpers::CreateComponent(this, &CableComp, "CableComp", MeshComp);
	CableComp->SetRelativeLocation(FVector(0, 0, -80));
	CableComp->CableWidth = 3.0f;
	CableComp->NumSegments = 20.0f;
	CableComp->EndLocation = GetActorLocation();
	CableComp->SolverIterations = 16.0f;
	CableComp->CableGravityScale = 0.5f;

	UMaterialInterface* material;
	CHelpers::GetAsset<UMaterialInterface>(&material, "MaterialInstanceConstant'/Game/Materials/MI_Emissive_Blue.MI_Emissive_Blue'");
	CableComp->SetMaterial(0, material);
}

void ACBuildProjectile::BeginPlay()
{
	Super::BeginPlay();
	CableComp->SetVisibility(false);
}

void ACBuildProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Cable�� ���� ��ġ ���� (�÷��̾��� �����տ� ����)
	FVector location;
	if (!!OwnerCharacter && SocketName != "")
	{
		location = OwnerCharacter->GetMesh()->GetSocketLocation(SocketName);
		CableComp->SetWorldLocation(location);
	}

	CheckFalse(bCanMove);

	// ��ǥ �������� �̵�
	Rate += DeltaTime * Speed;
	Rate = FMath::Clamp(Rate , 0.0f, 1.0f);
	location = FMath::Lerp<FVector, float>(StartLocation, EndLocation, Rate);
	SetActorLocation(location);
	

	// ��ǥ ������ ����
	if (FMath::IsNearlyEqual(Rate, 1))
		bCanMove = false;
}

void ACBuildProjectile::BuildSpawn()
{
	// �ӽ÷� ����(Build�� �������� üũ �� ��ġ ��)
	Super::BuildSpawn();
}

void ACBuildProjectile::SetDirection(FVector start, FVector end, FName InSocketName)
{
	// ���ư� ���� ���� (Build�� Ȯ���Ǹ� ���� ��)
	SocketName = InSocketName;
	CableComp->SetVisibility(true);
	StartLocation = start;
	EndLocation = end;
	Speed = (EndLocation - StartLocation).Size();
	Speed = 6000.0f / Speed;

	bCanMove = true;
	SetActorLocation(StartLocation);
}

void ACBuildProjectile::BuildStart(ACharacter* InOwnerCharacter)
{
	// ��ġ ����
	OwnerCharacter = InOwnerCharacter;
	MeshComp->SetVisibility(true);

	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
	//MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);

	MeshComp->SetMaterial(0, OriginMaterial);

}

void ACBuildProjectile::BuildFinish()
{
}