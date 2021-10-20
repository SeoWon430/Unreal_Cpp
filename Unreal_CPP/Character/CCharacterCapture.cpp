
#include "CCharacterCapture.h"
#include "Global.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"

ACCharacterCapture::ACCharacterCapture()
{

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &SkeletalMesh, "SkeletalMesh", Scene);
	CHelpers::CreateComponent(this, &Capture, "Capture", Scene);
}

void ACCharacterCapture::BeginPlay()
{
	Super::BeginPlay();
	
}



void ACCharacterCapture::StartCapture()
{
	SkeletalMesh->SetVisibility(true);
	Capture->SetVisibility(true);
	//CustomTimeDilation = 1.0f;
}
void ACCharacterCapture::EndCapture()
{
	SkeletalMesh->SetVisibility(false);
	Capture->SetVisibility(false);
	//CustomTimeDilation = 1.0f;
}
