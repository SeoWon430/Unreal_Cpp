
#include "CThrowDestination.h"
#include "Global.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/DecalComponent.h"

ACThrowDestination::ACThrowDestination()
{

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &MeshComp, "Mesh", Scene);
	CHelpers::CreateComponent(this, &DecalComp, "Decal", Scene);
	UStaticMesh* mesh;
	CHelpers::GetAsset<UStaticMesh>(&mesh, "StaticMesh'/Game/StaticMeshes/SM_Sphere.SM_Sphere'");
	MeshComp->SetStaticMesh(mesh);
	MeshComp->SetRelativeScale3D(FVector(5));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	UMaterialInstanceConstant* material;
	CHelpers::GetAsset<UMaterialInstanceConstant>(&material, "MaterialInstanceConstant'/Game/Objects/Trajectory/Material/MI_TrajectorySphere.MI_TrajectorySphere'");
	MeshComp->SetMaterial(0, material);

	CHelpers::GetAsset<UMaterialInstanceConstant>(&material, "MaterialInstanceConstant'/Game/Objects/Trajectory/Material/MI_TrajectoryTarget.MI_TrajectoryTarget'");
	DecalComp->SetDecalMaterial(material);
	//DecalComp->SetRelativeRotation(FRotator(90, 0, 0));

}

void ACThrowDestination::BeginPlay()
{
	Super::BeginPlay();

	UMaterialInstanceConstant* material = Cast<UMaterialInstanceConstant>(MeshComp->GetMaterial(0));
	UMaterialInstanceDynamic* dynamicMaterial = UMaterialInstanceDynamic::Create(material, this);
	MeshComp->SetMaterial(0, dynamicMaterial);
}


void ACThrowDestination::SetVisible()
{
	MeshComp->SetVisibility(true);
	DecalComp->SetVisibility(true);
	MeshComp->Activate();
	DecalComp->Activate();
}

void ACThrowDestination::SetHide()
{
	MeshComp->SetVisibility(false);
	DecalComp->SetVisibility(false);
	MeshComp->Deactivate();
	DecalComp->Deactivate();
}

void ACThrowDestination::SetDestination(FVector InLocation, FVector InNormal)
{
	// 닿는 면의 방향에 맞게 회전
	//	=> Decal을 바르게 그리기 위해
	DecalComp->SetWorldLocation(InLocation);
	MeshComp->SetWorldLocation(InLocation);

	FVector v1 = UKismetMathLibrary::Cross_VectorVector(InNormal, GetActorUpVector());
	FVector v2 = UKismetMathLibrary::Cross_VectorVector(v1, InNormal);
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(InLocation, InLocation + InNormal);
	DecalComp->SetWorldRotation(rot);


}
