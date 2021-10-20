#include "CBuildObject.h"
#include "Global.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "GameFramework/Character.h"

ACBuildObject::ACBuildObject()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &MeshComp, "Mesh", Scene);
	UStaticMesh* mesh;
	CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/Objects/SM_Cube.SM_Cube'");
	MeshComp->SetStaticMesh(mesh);
	MeshComp->SetRelativeLocation(FVector(0, 0, 50));
}

void ACBuildObject::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceConstant* material;
	CHelpers::GetAssetDynamic<UMaterialInstanceConstant>(&material, "MaterialInstanceConstant'/Game/Objects/MI_Default.MI_Default'");
	Material = UMaterialInstanceDynamic::Create(material, this);
	OriginMaterial = MeshComp->GetMaterial(0);
}

void ACBuildObject::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ACBuildObject::BuildSpawn()
{
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	MeshComp->SetMaterial(0, Material);
}

void ACBuildObject::BuildStart(ACharacter* InOwnerCharacter)
{
	OwnerCharacter = InOwnerCharacter;
	MeshComp->SetVisibility(false);
}

void ACBuildObject::BuildFinish()
{
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	MeshComp->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
	MeshComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);

	MeshComp->SetMaterial(0, OriginMaterial);
	MeshComp->SetVisibility(true);
}

void ACBuildObject::BuildCheck(bool InCanBuild)
{
	FLinearColor InColor;
	if (InCanBuild)
		InColor = FLinearColor(0, 1, 0);
	else
		InColor = FLinearColor(1, 0, 0);

	Material->SetVectorParameterValue("BaseColor", InColor);
}

