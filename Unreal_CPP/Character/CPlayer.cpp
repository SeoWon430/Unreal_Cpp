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

		// CameraCapture : 플레이어 죽을때, 화면 깨짐 효과
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
	//	=> PostProcess에 적용
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

	// 상태 변화시 호출
	//	=> 피격, 죽음 상태로 변환 시 호출될 함수 바인딩
	StateComp->OnStateTypeChanged.AddDynamic(this, &ACPlayer::OnStateTypeChanged);

	bUseControllerRotationYaw = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;

	bWallCheck = true;
	bCanWall = false;

	// 위젯 업데이트
	WidgetComp->UpdateHealth(StatusComp->GetHealth(), StatusComp->GetMaxHealth());
	WidgetComp->UpdateStamina(StatusComp->GetStamina(), StatusComp->GetMaxStamina());
}


void ACPlayer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Scan에 사용될 타임라인
	Timeline.TickTimeline(DeltaTime);

	// Action(플레이어 스킬)의 쿨타임 업데이트
	TArray<float> times = ActionComp->CoolTimes();
	WidgetComp->UpdateCoolTimes(times);

	// 벽타기
	WallCheck();
}


// #########################################################################################################
// #########################################################################################################
// #########################################################################################################
// 벽타기
//	=> 벽타기 가능상태에서 캐릭터가 정면으로 벽에 닿으면 벽에 수직으로 붙음
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
	FHitResult hitResult0 = FHitResult();		// 캐릭터 바닥 체크
	FHitResult hitResult1 = FHitResult();		// 캐릭터 상단 기준 정면 충돌점
	FHitResult hitResult2 = FHitResult();		// 캐릭터 하단 기준 정면 충돌점
	bool bHit1 = false;
	bool bHit2 = false;



	// 0. 벽에 있을 때, 바닥에 닿는 면이 없으면 벽타기 종료
	{
		start = GetActorLocation();
		end = start - GetActorUpVector() * 120.0f;
		if (bOnWall && !UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult0, true, FLinearColor::Red, FLinearColor::Red, 50))
		{

			// 처음 판단은 무시 (WireAction에 의해 갑자기 벽에 정면으로 닿으면 무시 될수 있기 때문)
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


	// 1. 정면에 벽이 있는지 판단
	{
		// 캐릭터 상단 기준 정면 라인충돌 (중심에서 Up+50) => 캐릭터 앞의 벽이 있을 때, 어느정도 높이 이상이여야만 벽을 탈 것
		//		=> 지상에서만 필요함
		start = GetActorLocation() + GetActorUpVector() * 50;
		end = start + GetActorForwardVector() * 100.0f;
		if (bOnWall == false)
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult1, true, FLinearColor::Red, FLinearColor::Red, 50))
				bHit1 = true;


		// 캐릭터 하단 기준 정면 라인충돌 (중심에서 Up-40) => 캐릭터 앞의 벽이 있을 때, 벽을 탄 후 설정할 위치값에 필요
		start = GetActorLocation() - GetActorUpVector() * 40;
		end = start + GetActorForwardVector() * 100.0f;
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForOneFrame, hitResult2, true, FLinearColor::Red, FLinearColor::Red, 50))
		{
			bHit2 = true;
			//WallNormal = hitResult2.Normal;
			WallNormal = hitResult2.ImpactNormal;
		}

		// 1-1. 지상에 있을 때, 위 두 라인충돌에서 닿는 것(벽)이 없으면 종료
		if (bOnWall == false)
		{
			if (!bHit1 || !bHit2)
			{
				return;
			}
		}
		// 1-2. 벽에 있을 때, 하단의 라인충돌에만 닿는것이 없어도 종료
		else if (!bHit2)
		{
			// 현재 밟고 있는 벽의 Normal과 이미 기록한 Normal이 다를 경우 바닥 각도 다시 계산 (8도이상 차이 날 경우)
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



	// 2. 닿는 벽이 원래 수평 바닥일 때 (바닥면이 약 -45~45도이내)
	if (UKismetMathLibrary::Dot_VectorVector(WallNormal, FVector(0, 0, 1)) > 0.7f)
	{
		OffWall(hitResult2.Location + FVector(0, 0, 88) + GetActorUpVector() * 100);
		return;
	}



	FHitResult hitResult3;
	// 3. 닿는 벽이 원래 바닥이 아닌 경우
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


// 벽에 붙을 때 회전값 조정
void ACPlayer::OnWall(FVector InLocation)
{
	bOnWall = true;
	
	//UKismetMathLibrary::FindLookAtRotation()
	FRotator rotation = UKismetMathLibrary::MakeRotFromZX(WallNormal, WallOnForward);
	rotation += FRotator(0.05f);
	SetActorRotation(rotation);

	FVector location = InLocation + WallNormal * 88;
	SetActorLocation(location);


	// 카메라 시야 및 움직임 조정
	bUseControllerRotationYaw = false;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
	SpringArm->TargetArmLength = 480.0f;

	// 수평 이동에 의해 벽에서 다른벽으로 넘어갈때, 캐릭터 바라보는 방향 그대로 유지
	//	=> 위에서 캐릭터 회전값이 바뀌며 하늘을 바라보게 되기 때문에 다시 설정
	OnMoveRight(RMoveDirection);
	RMoveDirection = 0.0f;
}


// 벽에 닿아 있는 상태 유지
//	=> 계속 벽은 타고 있지만 벽의 각도가 바뀐경우 위치 다시 조정 (곡면 벽에 사용)
//	=> 현재 곡면 벽 이동시 끊겨 보이기에 추후 수정 @@@@@
void ACPlayer::OnWallMaintain(FVector InLocation)
{
	bOnWall = true;

	FVector location = InLocation + WallNormal * 88;
	SetActorLocation(location);
}


// 벽타기 종료
//	=> 회전값을 원래 상태로 돌림
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
// 키 매핑
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

// 수직 이동 (W,S)
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

// 수평 이동 (A,S)
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

// 수평 시야
void ACPlayer::OnHorizontalLook(float InAxis)
{
	CheckFalse(StatusComp->CanMove());
	//CheckTrue(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying);

	float rate = OptionComp->GetHorizontalLookRate();
	AddControllerYawInput(InAxis * rate * DilationTime * GetWorld()->GetDeltaSeconds());
}

// 수직 시야
void ACPlayer::OnVerticalLook(float InAxis)
{
	CheckFalse(StatusComp->CanMove());
	//CheckTrue(GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Flying);

	float rate = OptionComp->GetVerticalLookRate();
	AddControllerPitchInput(InAxis * rate * DilationTime * GetWorld()->GetDeltaSeconds());
}


// 메뉴 탭 (Tab)
void ACPlayer::OnMenu()
{
	WidgetComp->OnMenu();
}

// Scanning : 액터의 Custom Depth를 이용하여 적 탐색
//	=> PostProcess에 등록되어 사용
void ACPlayer::OnScan()
{
	CheckFalse(StateComp->IsIdleMode());
	CheckNull(ParameterCollection);

	FOnTimelineFloat scanning;
	scanning.BindUFunction(this, "OnScanning");		// 타임라인 동안 호출
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
	// Tick에서 TickTimeline해야 함
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
//	=> 스킬과 휴식, 빌드 등의 행동
void ACPlayer::OnAction()
{
	ActionComp->Action();
}

// 휴식 (X)
void ACPlayer::OnRest()
{
	CheckTrue(GetCharacterMovement()->IsFalling());
	if (StateComp->IsIdleMode() || ActionComp->IsRestMode())
		ActionComp->Action_Rest();
}

// 만들기 (F)
void ACPlayer::OnBuild()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Build();
}

// 벽 오르기or넘기 (V)
void ACPlayer::OnClimb()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Climb();
}

// 와이어 던지기&날라가기 (LShift)
void ACPlayer::OnWire()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Wire();
}

// 오브젝트 던지기 (G)
void ACPlayer::OnThrow()
{
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Throw();
}

// 벽타기 Toggle (LCtrl)
//	=> 벽타기 사용 여부
void ACPlayer::OnWallToggle()
{
	CheckFalse(StateComp->IsIdleMode());
	bCanWall = !bCanWall;

	if (bCanWall == false && bOnWall == true)
		OnJump();
}

// 점프 (Space)
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
		// 벽타는 도중에는 벽타기 종료
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

// 스킬1 (Q)
void ACPlayer::OnSkill1()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill1();
}

// 스킬2 (E)
void ACPlayer::OnSkill2()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill2();

}

// 스킬3 (R)
void ACPlayer::OnSkill3()
{
	CheckTrue(bOnWall);
	CheckFalse(StateComp->IsIdleMode());
	ActionComp->Action_Skill3();
}

// 스킬4 (T)
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
// 기타 함수

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

	// 히트리커버리가 높으면 피격모션 무시
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

// 플레이어 죽음
void ACPlayer::End_Dead()
{
	ActionComp->End_Dead();


	Capture->CaptureScene();

	// 플레이어가 죽었을 때 화면 깨짐 효과를 위한 액터 스폰
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
	
	// 위에서 스폰한 액터에서 게임을 종료 시킴
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