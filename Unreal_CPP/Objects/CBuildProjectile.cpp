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

	// Cable의 시작 위치 설정 (플레이어의 오른손에 고정)
	FVector location;
	if (!!OwnerCharacter && SocketName != "")
	{
		location = OwnerCharacter->GetMesh()->GetSocketLocation(SocketName);
		CableComp->SetWorldLocation(location);
	}

	CheckFalse(bCanMove);

	// 목표 지점으로 이동
	Rate += DeltaTime * Speed;
	Rate = FMath::Clamp(Rate , 0.0f, 1.0f);
	location = FMath::Lerp<FVector, float>(StartLocation, EndLocation, Rate);
	SetActorLocation(location);
	

	// 목표 지점에 도착
	if (FMath::IsNearlyEqual(Rate, 1))
		bCanMove = false;
}

void ACBuildProjectile::BuildSpawn()
{
	// 임시로 스폰(Build가 가능한지 체크 후 설치 됨)
	Super::BuildSpawn();
}

void ACBuildProjectile::SetDirection(FVector start, FVector end, FName InSocketName)
{
	// 날아갈 방향 설정 (Build가 확정되면 실행 됨)
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
	// 설치 시작
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