#include "CSceneCapture2D.h"
#include "Global.h"
#include "DestructibleMesh.h"
#include "DestructibleComponent.h"
#include "Components/SceneCaptureComponent2D.h"


ACSceneCapture2D::ACSceneCapture2D()
{
	CHelpers::CreateComponent(this, &DestructMeshComp, "DestructMeshComp", RootComponent);

	USkeletalMesh* mesh;
	// DestructibleMesh는 USkeletalMesh로 얻어오기
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
	// 이 액터는 카메라 바로 앞에 생성되어 깨짐
	DestructMeshComp->ApplyDamage(500, GetActorLocation(), GetActorForwardVector(), 1.0f);
	DestructMeshComp->ApplyRadiusDamage(50000, GetActorLocation(), 1000, 100, true);

	UKismetSystemLibrary::K2_SetTimer(this, "End", 2.0f, false);
}


void ACSceneCapture2D::End()
{
	// 플레이어가 죽었기에 게임 종료
	APlayerController* controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	UKismetSystemLibrary::QuitGame(GetWorld(), controller, EQuitPreference::Quit, false);	
}