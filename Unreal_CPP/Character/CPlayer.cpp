#include "CPlayer.h"
#include "Global.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/PostProcessComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/COptionComponent.h"
#include "Components/CMontagesComponent.h"
#include "Components/CActionComponent.h"
#include "Components/CWidgetPlayerComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialParameterCollection.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Engine/PostProcessVolume.h"
#include "PostProcess/CPostProcessVolume.h"
#include "Actions/CAction.h"
#include "CSceneCapture2D.h"
#include "Widget/CUserWidget_PlayerStatus.h"
#include "Widget/CUserWidget_SkillSet.h"



ACPlayer::ACPlayer()
{
	PrimaryActorTick.bCanEverTick = true;

	// Create Component
	{
		// Create SceneComponent
		CHelpers::CreateComponent(this, &SpringArm, "SpringArm", GetMesh());
		CHelpers::CreateComponent(this, &Camera, "Camera", SpringArm);
		CHelpers::CreateComponent(this, &Capture, "Capture", Camera);
		CHelpers::CreateComponent(this, &PostProcess, "PostProcess", GetRootComponent());

		// Create ActorComponent
		CHelpers::CreateActorComponent(this, &ActionComp, "Action");
		CHelpers::CreateActorComponent(this, &StatusComp, "Status");
		CHelpers::CreateActorComponent(this, &OptionComp, "Option");
		CHelpers::CreateActorComponent(this, &WidgetComp, "Widget");
		CHelpers::CreateActorComponent(this, &MontagesComp, "Montages");
		CHelpers::CreateActorComponent(this, &StateComp, "State");

	}

	// Component Settings
	{
		// Set Mesh
		USkeletalMesh* mesh;
		CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
		//CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Genshin/Zhongli/Mesh/SK_Zhongli.SK_Zhongli'");
		GetMesh()->SetSkeletalMesh(mesh);
		GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		// Set AnimInstace
		TSubclassOf<UAnimInstance> animClass;
		CHelpers::GetClass<UAnimInstance>(&animClass, "AnimBlueprint'/Game/Player/ABP_CPlayer.ABP_CPlayer_C'");
		GetMesh()->SetAnimInstanceClass(animClass);

		// Set Camera
		bUseControllerRotationYaw = false;
		SpringArm->SetRelativeLocation(FVector(-30, 0, 160));
		SpringArm->SetRelativeRotation(FRotator(0, 90, 0));
		SpringArm->TargetArmLength = 300.0f;
		SpringArm->bUsePawnControlRotation = true;
		SpringArm->bDoCollisionTest = false;
		SpringArm->bEnableCameraLag = true;

		// CameraCapture : �÷��̾� ������, ȭ�� ���� ȿ��
		UTextureRenderTarget2D* renderTarget;
		CHelpers::GetAsset<UTextureRenderTarget2D>(&renderTarget, "TextureRenderTarget2D'/Game/Player/RT_PlayerCamera.RT_PlayerCamera'");
		Capture->TextureTarget = renderTarget;
		Capture->CaptureSource = ESceneCaptureSource::SCS_FinalColorLDR;
		Capture->bCaptureEveryFrame = false;
		Capture->bCaptureOnMovement = false;

		// Set Movement
		StatusComp->SetSpeed(ECharacterSpeed::Sprint);
		//GetCharacterMovement()->MaxWalkSpeed = StatusComp->GetSprintSpeed();
		GetCharacterMovement()->MaxFlySpeed = StatusComp->GetSprintSpeed();
		GetCharacterMovement()->bOrientRotationToMovement = true;
		GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);
		GetCharacterMovement()->BrakingDecelerationFlying = GetCharacterMovement()->BrakingDecelerationWalking;
		GetCharacterMovement()->JumpZVelocity = 500.0f;

	
	}


	GetMesh()->bRenderCustomDepth = true;
	GetMesh()->CustomDepthStencilValue = 1.0f;

	// Set Scanning
	//	=> PostProcess�� ����
	{
		
		FWeightedBlendable blend;
		UMaterialInstanceConstant* scanMaterial;
		CHelpers::GetAsset<UMaterialInstanceConstant>(&scanMaterial, "MaterialInstanceConstant'/Game/Player/Scanning/M_Scan_Inst.M_Scan_Inst'");
		blend.Object = scanMaterial;
		blend.Weight = 1.0f;
		PostProcess->Settings.WeightedBlendables.Array.Add(blend);
		
		FWeightedBlendable skill5Effect;
		UMaterialInstanceConstant* stencilMaterial;
		CHelpers::GetAsset<UMaterialInstanceConstant>(&stencilMaterial, "MaterialInstanceConstant'/Game/Materials/PostProcess/M_BlackOut_Inst.M_BlackOut_Inst'");
		skill5Effect.Object = stencilMaterial;
		skill5Effect.Weight = 0.0f;
		PostProcess->Settings.WeightedBlendables.Array.Add(skill5Effect);

		CHelpers::GetAsset<UCurveFloat>(&Curve, "CurveFloat'/Game/Camera/Curve_CameraPath.Curve_CameraPath'");
		CHelpers::GetAsset<UMaterialParameterCollection>(&ParameterCollection, "MaterialParameterCollection'/Game/Player/Scanning/MPC_Scan.MPC_Scan'");
	}
}


void ACPlayer::BeginPlay()
{
	Super::BeginPlay();

	// ���� ��ȭ�� ȣ��
	//	=> �ǰ�, ���� ���·� ��ȯ �� ȣ��� �Լ� ���ε�
	StateComp->OnStateTypeChanged.AddDynamic(this, &ACPlayer::OnStateTypeChanged);

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	bWallCheck = true;
	bCanWall = false;

	// ���� ������Ʈ
	WidgetComp->UpdateHealth(StatusComp->GetHealth(), StatusComp->GetMaxHealth());
	WidgetComp->UpdateStamina(StatusComp->GetStamina(), StatusComp->GetMaxStamina());
}


void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Scan�� ���� Ÿ�Ӷ���
	Timeline.TickTimeline(DeltaTime);

	// Action(�÷��̾� ��ų)�� ��Ÿ�� ������Ʈ
	TArray<float> times = ActionComp->CoolTimes();
	WidgetComp->UpdateCoolTimes(times);

	// ��Ÿ��
	WallCheck();
}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ��Ÿ��
//	=> ��Ÿ�� ���ɻ��¿��� ĳ���Ͱ� �������� ���� ������ ���� �������� ����
void ACPlayer::WallCheck()
{
	CheckFalse(bCanWall);
	CheckFalse(bWallCheck);


	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ignores;
	ignores.Add(this);

	FVector start;
	FVector end;
	FHitResult hitResult0 = FHitResult();		// ĳ���� �ٴ� üũ
	FHitResult hitResult1 = FHitResult();		// ĳ���� ��� ���� ���� �浹��
	FHitResult hitResult2 = FHitResult();		// ĳ���� �ϴ� ���� ���� �浹��
	bool bHit1 = false;
	bool bHit2 = false;



	// 0. ���� ���� ��, �ٴڿ� ��� ���� ������ ��Ÿ�� ����
	{
		start = GetActorLocation();
		end = start - GetActorUpVector() * 120.0f;
		if (bOnWall && !UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult0, true, FLinearColor::Red, FLinearColor::Red, 50))
		{

			// ó�� �Ǵ��� ���� (WireAction�� ���� ���ڱ� ���� �������� ������ ���� �ɼ� �ֱ� ����)
			if (bWallNone == false)
				bWallNone = true;
			else
			{
				OnJump();
				bWallNone = false;
				return;
			}

		}
	}


	// 1. ���鿡 ���� �ִ��� �Ǵ�
	{
		// ĳ���� ��� ���� ���� �����浹 (�߽ɿ��� Up+50) => ĳ���� ���� ���� ���� ��, ������� ���� �̻��̿��߸� ���� Ż ��
		//		=> ���󿡼��� �ʿ���
		start = GetActorLocation() + GetActorUpVector() * 50;
		end = start + GetActorForwardVector() * 100.0f;
		if (bOnWall == false)
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult1, true, FLinearColor::Red, FLinearColor::Red, 50))
				bHit1 = true;


		// ĳ���� �ϴ� ���� ���� �����浹 (�߽ɿ��� Up-40) => ĳ���� ���� ���� ���� ��, ���� ź �� ������ ��ġ���� �ʿ�
		start = GetActorLocation() - GetActorUpVector() * 40;
		end = start + GetActorForwardVector() * 100.0f;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult2, true, FLinearColor::Red, FLinearColor::Red, 50))
		{
			bHit2 = true;
			//WallNormal = hitResult2.Normal;
			WallNormal = hitResult2.ImpactNormal;
		}

		// 1-1. ���� ���� ��, �� �� �����浹���� ��� ��(��)�� ������ ����
		if (bOnWall == false)
		{
			if (!bHit1 || !bHit2)
			{
				return;
			}
		}
		// 1-2. ���� ���� ��, �ϴ��� �����浹���� ��°��� ��� ����
		else if (!bHit2)
		{
			// ���� ��� �ִ� ���� Normal�� �̹� ����� Normal�� �ٸ� ��� �ٴ� ���� �ٽ� ��� (8���̻� ���� �� ���)
			if (!!hitResult0.GetActor())
			{
				if (UKismetMathLibrary::Dot_VectorVector(WallNormal, hitResult0.ImpactNormal) < 0.99f)
				{
					WallNormal = hitResult0.ImpactNormal;

					start = GetActorLocation() + FVector(0, 0, 10);
					end = start - GetActorUpVector() * 120.0f;
					if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForDuration, hitResult2, true, FLinearColor::Blue, FLinearColor::Red, 50))
					{
						WallOnRight = (hitResult1.ImpactPoint - hitResult0.ImpactPoint).GetSafeNormal();
						WallOnRight = UKismetMathLibrary::Cross_VectorVector(WallNormal, WallOnForward).GetSafeNormal();

						OnWall(hitResult0.ImpactPoint);
					}
				}
				OnWallMaintain(hitResult0.ImpactPoint);
			}
			return;
		}
	}



	// 2. ��� ���� ���� ���� �ٴ��� �� (�ٴڸ��� �� -45~45���̳�)
	if (UKismetMathLibrary::Dot_VectorVector(WallNormal, FVector(0, 0, 1)) > 0.7f)
	{
		OffWall(hitResult2.Location + FVector(0, 0, 88) + GetActorUpVector() * 100);
		return;
	}



	FHitResult hitResult3;
	// 3. ��� ���� ���� �ٴ��� �ƴ� ���
	{
		start = GetActorLocation() - GetActorUpVector() * 40 + FVector(0, 0, 10);
		end = start + GetActorForwardVector() * 100.0f;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForDuration, hitResult3, true, FLinearColor::Blue, FLinearColor::Red, 50))
		{

			WallOnForward = (hitResult3.ImpactPoint - hitResult2.ImpactPoint).GetSafeNormal();
			WallOnRight = UKismetMathLibrary::Cross_VectorVector(WallNormal, WallOnForward).GetSafeNormal();

			OnWall(hitResult2.ImpactPoint);

			//DrawDebugLine(GetWorld(), hitResult2.Location, hitResult2.Location + WallNormal * 50, FColor::Cyan, false, 10, 0, 5);
			//DrawDebugLine(GetWorld(), hitResult2.Location, hitResult2.Location + WallOnForward * 50, FColor::Magenta, false, 10, 0, 5);
			//DrawDebugLine(GetWorld(), hitResult2.Location, hitResult2.Location + WallOnRight * 50, FColor::Yellow, false, 10, 0, 5);
			return;
		}
	}
}


// ���� ���� �� ȸ���� ����
void ACPlayer::OnWall(FVector InLocation)
{
	bOnWall = true;
	
	//UKismetMathLibrary::FindLookAtRotation()
	FRotator rotation = UKismetMathLibrary::MakeRotFromZX(WallNormal, WallOnForward);
	rotation += FRotator(0.05f);
	SetActorRotation(rotation);

	FVector location = InLocation + WallNormal * 88;
	SetActorLocation(location);


	// ī�޶� �þ� �� ������ ����
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	SpringArm->TargetArmLength = 480.0f;

	// ���� �̵��� ���� ������ �ٸ������� �Ѿ��, ĳ���� �ٶ󺸴� ���� �״�� ����
	//	=> ������ ĳ���� ȸ������ �ٲ�� �ϴ��� �ٶ󺸰� �Ǳ� ������ �ٽ� ����
	OnMoveRight(RMoveDirection);
	RMoveDirection = 0.0f;
}


// ���� ��� �ִ� ���� ����
//	=> ��� ���� Ÿ�� ������ ���� ������ �ٲ��� ��ġ �ٽ� ���� (��� ���� ���)
//	=> ���� ��� �� �̵��� ���� ���̱⿡ ���� ���� @@@@@
void ACPlayer::OnWallMaintain(FVector InLocation)
{
	bOnWall = true;

	FVector location = InLocation + WallNormal * 88;
	SetActorLocation(location);
}


// ��Ÿ�� ����
//	=> ȸ������ ���� ���·� ����
void ACPlayer::OffWall(FVector InLocation)
{
	FVector right = UKismetMathLibrary::Cross_VectorVector(FVector(0, 0, 1), GetActorUpVector());
	FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(GetActorUpVector(), right, FVector(0, 0, 1));
	//FRotator rotation = UKismetMathLibrary::MakeRotationFromAxes(WallNormal, WallOnRight, FVector(0, 0, 1));
	rotation.Roll = 0;
	rotation.Pitch = 0;
	SetActorRotation(rotation);
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	SetActorLocation(InLocation);
	//SetActorLocation(GetActorLocation() + GetActorForwardVector() * 100);

	bOnWall = false;
	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	SpringArm->SetRelativeLocation(FVector(-30, 0, 160));
	SpringArm->TargetArmLength = 300.0f;

}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// Ű ����
void ACPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Bind Axis
	PlayerInputComponent->BindAxis("MoveForward", this, &ACPlayer::OnMoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACPlayer::OnMoveRight);
	PlayerInputComponent->BindAxis("HorizontalLook", this, &ACPlayer::OnHorizontalLook);
	PlayerInputComponent->BindAxis("VerticalLook", this, &ACPlayer::OnVerticalLook);

	// Bind Action
	PlayerInputComponent->BindAction("Rest", EInputEvent::IE_Pressed, this, &ACPlayer::OnRest);
	PlayerInputComponent->BindAction("Build", EInputEvent::IE_Pressed, this, &ACPlayer::OnBuild);
	PlayerInputComponent->BindAction("Action", EInputEvent::IE_Pressed, this, &ACPlayer::OnAction);
	PlayerInputComponent->BindAction("Climb", EInputEvent::IE_Pressed, this, &ACPlayer::OnClimb);
	PlayerInputComponent->BindAction("Wire", EInputEvent::IE_Pressed, this, &ACPlayer::OnWire);
	PlayerInputComponent->BindAction("Throw", EInputEvent::IE_Pressed, this, &ACPlayer::OnThrow);
	PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACPlayer::OnJump);
	PlayerInputComponent->BindAction("WallToggle", EInputEvent::IE_Pressed, this, &ACPlayer::OnWallToggle);
	PlayerInputComponent->BindAction("Menu", EInputEvent::IE_Pressed, this, &ACPlayer::OnMenu);
	PlayerInputComponent->BindAction("Scan", EInputEvent::IE_Released, this, &ACPlayer::OnScan);

	PlayerInputComponent->BindAction("Skill1", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill1);
	PlayerInputComponent->BindAction("Skill2", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill2);
	PlayerInputComponent->BindAction("Skill3", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill3);

	PlayerInputComponent->BindAction("Skill4", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill4);
	PlayerInputComponent->BindAction("Skill4", EInputEvent::IE_Released, this, &ACPlayer::OffSkill4);
	PlayerInputComponent->BindAction("Skill5", EInputEvent::IE_Pressed, this, &ACPlayer::OnSkill5);

}

// ���� �̵� (W,S)
void ACPlayer::OnMoveForward(float InAxis)
{
	CheckFalse(StatusComp->CanMove());

	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	if (bOnWall) {
		rotator = UKismetMathLibrary::MakeRotFromZX(WallNormal, WallOnForward);
		GetMesh()->SetRelativeRotation(FRotator(0, 0, 0));

		if (InAxis > 0.01f)
			SetActorRotation(UKismetMathLibrary::MakeRotFromXY(WallOnForward, WallOnRight));
		else if (InAxis < -0.01f)
			SetActorRotation(UKismetMathLibrary::MakeRotFromXY(-WallOnForward, -WallOnRight));
	}
	FVector direction = FQuat(rotator).GetForwardVector();
	AddMovementInput(direction, InAxis);

}

// ���� �̵� (A,S)
void ACPlayer::OnMoveRight(float InAxis)
{

	CheckFalse(StatusComp->CanMove());

	FRotator rotator = FRotator(0, GetControlRotation().Yaw, 0);
	if (bOnWall) {
		rotator = UKismetMathLibrary::MakeRotFromZX(WallNormal, WallOnForward);
		GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));

		if (InAxis > 0.1f)
		{
			SetActorRotation(UKismetMathLibrary::MakeRotFromYX(-WallOnForward, WallOnRight));
			RMoveDirection = 1.0f;
		}
		else if (InAxis < -0.1f)
		{
			SetActorRotation(UKismetMathLibrary::MakeRotFromYX(WallOnForward, -WallOnRight));
			RMoveDirection = -1.0f;
		}
	}
	FVector direction = FQuat(rotator).GetRightVector();
	AddMovementInput(direction, InAxis);
}

// ���� �þ�
void ACPlayer::OnHorizontalLook(float InAxis)
{
	CheckFalse(StatusComp->CanMove());
	//CheckTrue(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying);

	float rate = OptionComp->GetHorizontalLookRate();
	AddControllerYawInput(InAxis * rate * DilationTime * GetWorld()->GetDeltaSeconds());
}

// ���� �þ�
void ACPlayer::OnVerticalLook(float InAxis)
{
	CheckFalse(StatusComp->CanMove());
	//CheckTrue(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying);

	float rate = OptionComp->GetVerticalLookRate();
	AddControllerPitchInput(InAxis * rate * DilationTime * GetWorld()->GetDeltaSeconds());
}


// �޴� �� (Tab)
void ACPlayer::OnMenu()
{
	WidgetComp->OnMenu();
}

// Scanning : ������ Custom Depth�� �̿��Ͽ� �� Ž��
//	=> PostProcess�� ��ϵǾ� ���
void ACPlayer::OnScan()
{
	CheckFalse(StateComp->IsIdleMode());
	CheckNull(ParameterCollection);

	FOnTimelineFloat scanning;
	scanning.BindUFunction(this, "OnScanning");		// Ÿ�Ӷ��� ���� ȣ��
	Timeline = FTimeline();
	Timeline.AddInterpFloat(Curve, scanning);
	Timeline.SetTimelineLengthMode(ETimelineLengthMode::TL_LastKeyFrame);
	Timeline.SetPlayRate(1.0f);
	Timeline.PlayFromStart();

	UKismetSystemLibrary::K2_SetTimer(this, "OffScan", 5.0f, false);
}

void ACPlayer::OffScan()
{
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), ParameterCollection, "Radius", 0);
}

void ACPlayer::OnScanning(float Output)
{
	// Tick���� TickTimeline�ؾ� ��
	UKismetMaterialLibrary::SetScalarParameterValue(GetWorld(), ParameterCollection, "Radius", Output);
}

void ACPlayer::OnBlackOut()
{
	PostProcess->Settings.WeightedBlendables.Array[1].Weight = 1.0f;

}

void ACPlayer::OffBlackOut()
{
	PostProcess->Settings.WeightedBlendables.Array[1].Weight = 0.0f;
}

// Action
//	=> ��ų�� �޽�, ���� ���� �ൿ
void ACPlayer::OnAction()
{
	ActionComp->Action();
}

// �޽� (X)
void ACPlayer::OnRest()
{
	CheckTrue(GetCharacterMovement()->IsFalling());
	if (StateComp->IsIdleMode() || ActionComp->IsRestMode())
		ActionComp->Action_Rest();
}

// ����� (F)
void ACPlayer::OnBuild()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Build();
}

// �� ������or�ѱ� (V)
void ACPlayer::OnClimb()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Climb();
}

// ���̾� ������&���󰡱� (LShift)
void ACPlayer::OnWire()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Wire();
}

// ������Ʈ ������ (G)
void ACPlayer::OnThrow()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Throw();
}

// ��Ÿ�� Toggle (LCtrl)
//	=> ��Ÿ�� ��� ����
void ACPlayer::OnWallToggle()
{
	CheckFalse(StateComp->IsIdleMode());
	bCanWall = !bCanWall;

	if (bCanWall == false && bOnWall == true)
		OnJump();
}

// ���� (Space)
void ACPlayer::OnJump()
{
	CheckTrue(GetCharacterMovement()->IsFalling());
	CheckFalse(StateComp->IsIdleMode());

	StateComp->SetJumpMode();
	StatusComp->SetStop();
	bWallCheck = false;

	if (bOnWall == false)
	{
		MontagesComp->PlayJump();
		Jump();
	}
	else
	{
		// ��Ÿ�� ���߿��� ��Ÿ�� ����
		FVector location = GetActorLocation() - GetActorUpVector() * 88;
		FVector direction;
		if (GetInputAxisValue("MoveForward") > 0.5f)
		{
			MontagesComp->PlayJump();
			direction = FVector(0, 0, 600);

		}
		else
		{
			MontagesComp->PlayTurn();
			direction = WallNormal * 250 + WallOnForward * 300;
		}

		OffWall(location);
		LaunchCharacter(direction, false, false);
	}
}

void ACPlayer::OffJump()
{
	StateComp->SetIdleMode();
	StatusComp->SetMove();
	StopJumping();
	bWallCheck = true;
}

// ��ų1 (Q)
void ACPlayer::OnSkill1()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill1();
}

// ��ų2 (E)
void ACPlayer::OnSkill2()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill2();

}

// ��ų3 (R)
void ACPlayer::OnSkill3()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill3();
}

// ��ų4 (T)
void ACPlayer::OnSkill4()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill4();
}

void ACPlayer::OffSkill4()
{
	CheckFalse(ActionComp->IsSkill4Mode());
	ActionComp->UnAction_Skill4();
}

void ACPlayer::OnSkill5()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill5();
}



// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// ��Ÿ �Լ�

FGenericTeamId ACPlayer::GetGenericTeamId() const
{
	return FGenericTeamId(TeamID);
}

FVector ACPlayer::CameraDirection()
{
	return Camera->GetForwardVector();
}

void ACPlayer::SetCameraFar()
{
	SpringArm->TargetArmLength = 400.0f;
}

void ACPlayer::SetCameraFarReset()
{
	SpringArm->TargetArmLength = 300.0f;
}

void ACPlayer::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
	case EStateType::Hitted: Hitted(); break;
	case EStateType::Dead: Dead(); break;
	}
}


void ACPlayer::SetDilationTime(float InRate)
{
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), InRate);

	DilationTime = 1 / InRate;

	TArray<AActor*> volumes;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACPostProcessVolume::StaticClass(), volumes);
	CheckFalse(volumes.Num() > 0);
	ACPostProcessVolume* post = Cast<ACPostProcessVolume>(volumes[0]);

	if (InRate > 0.99f)
		post->SetGray(0.0f);
	else
		post->SetGray(1.0f);
}




float ACPlayer::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	CLog::Print(damage);
	damage *= (1.0f - StatusComp->GetDef()*0.01f );
	DamageInstigator = EventInstigator;

	
	ActionComp->AbortByDamage();
	CLog::Print("Player : " + DamageInstigator->GetName());

	CLog::Print(damage);
	StatusComp->SubHealth(damage);
	WidgetComp->UpdateHealth(StatusComp->GetHealth(), StatusComp->GetMaxHealth());

	if (StatusComp->GetHealth() <= 0.0f)
	{
		StateComp->SetDeadMode();
		return 0.0f;
	}

	// ��Ʈ��Ŀ������ ������ �ǰݸ�� ����
	float hitRecovery = StatusComp->GetHitRecovery();
	if (hitRecovery < 2.0f) {
		StateComp->SetHittedMode();
		CLog::Print("Hit");

	}

	
	return StatusComp->GetHealth();
}


void ACPlayer::Hitted()
{
	StatusComp->SetMove();
	MontagesComp->PlayHitted(StatusComp->GetHitRecovery());
}

void ACPlayer::Dead()
{
	CheckFalse(StateComp->IsDeadMode());

	ActionComp->Reset();
	MontagesComp->PlayDead();
	StatusComp->SetStop();
	GetCapsuleComponent()->SetCollisionProfileName("Spectator");

	UKismetSystemLibrary::K2_SetTimer(this, "End_Dead", 2.0f, false);
}

// �÷��̾� ����
void ACPlayer::End_Dead()
{
	ActionComp->End_Dead();


	Capture->CaptureScene();

	// �÷��̾ �׾��� �� ȭ�� ���� ȿ���� ���� ���� ����
	FTransform transform = Camera->GetComponentTransform();
	transform.SetLocation(transform.GetLocation() + Camera->GetForwardVector() * 24.0f);
	ACSceneCapture2D* scene = GetWorld()->SpawnActorDeferred<ACSceneCapture2D>
		(
			ACSceneCapture2D::StaticClass(),
			transform,
			this,
			nullptr,
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn
			);
	UGameplayStatics::FinishSpawningActor(scene, transform);
	
	// ������ ������ ���Ϳ��� ������ ���� ��Ŵ
	//UKismetSystemLibrary::QuitGame(GetWorld(), GetController<APlayerController>(), EQuitPreference::Quit, false);
}

void ACPlayer::CollisionOff()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ACPlayer::CollisionOn()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
}


void ACPlayer::UpdateStatusWidget()
{
	WidgetComp->UpdateHealth(StatusComp->GetHealth(), StatusComp->GetMaxHealth());
	WidgetComp->UpdateStamina(StatusComp->GetStamina(), StatusComp->GetMaxStamina());
}