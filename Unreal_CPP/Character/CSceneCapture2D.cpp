#include "CSceneCapture2D.h"
#include "Global.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"
#include "Components/SceneCaptureComponent2D.h"


ACSceneCapture2D::ACSceneCapture2D()
{
	CHelpers::CreateComponent(this, &DestructMeshComp, "DestructMeshComp", RootComponent);

	USkeletalMesh* mesh;
	// DestructibleMesh�� USkeletalMesh�� ������
	CHelpers::GetAsset<USkeletalMesh>(&mesh, "DestructibleMesh'/Game/Player/RenderTarget/SM_RenderTarget_DM.SM_RenderTarget_DM'");
	UDestructibleMesh* destructMesh = Cast<UDestructibleMesh>(mesh);
	DestructMeshComp->SetDestructibleMesh(destructMesh);
	DestructMeshComp->SetRelativeLocation(FVector(80, 0, 0));
	DestructMeshComp->SetRelativeRotation(FRotator(0, -90, 0));
	DestructMeshComp->LargeChunkThreshold = 100.0f;
	DestructMeshComp->SetEnableGravity(false);

	DestructMeshComp->SetCollisionProfileName("NoCollision");
	DestructMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}


void ACSceneCapture2D::BeginPlay()
{
	// �� ���ʹ� ī�޶� �ٷ� �տ� �����Ǿ� ����
	DestructMeshComp->ApplyDamage(500, GetActorLocation(), GetActorForwardVector(), 1.0f);
	DestructMeshComp->ApplyRadiusDamage(50000, GetActorLocation(), 1000, 100, true);

	UKismetSystemLibrary::K2_SetTimer(this, "End", 2.0f, false);
}


void ACSceneCapture2D::End()
{
	// �÷��̾ �׾��⿡ ���� ����
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), controller, EQuitPreference::Quit, false);	
}