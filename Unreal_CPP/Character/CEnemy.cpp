#include "CEnemy.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/CStatusComponent.h"
#include "Components/CMontagesComponent.h"
#include "Components/CActionComponent.h"
#include "Components/CWidgetDamageTextComponent.h"
#include "Widget/CUserWidget_EnemyHealth.h"


ACEnemy::ACEnemy()
{
	// Create SceneComponent
	CHelpers::CreateComponent(this, &HealthWidget, "HealthWidget", GetMesh());
	CHelpers::CreateComponent(this, &DamageWidget, "DamageTextComp", GetMesh());

	// Create ActorComponent
	CHelpers::CreateActorComponent(this, &StatusComp, "StatusComp");
	CHelpers::CreateActorComponent(this, &MontagesComp, "MontagesComp");
	CHelpers::CreateActorComponent(this, &StateComp, "StateComp");
	CHelpers::CreateActorComponent(this, &ActionComp, "ActionComp");

	// Component Settings
	USkeletalMesh* mesh;
	CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
	GetMesh()->SetSkeletalMesh(mesh);
	GetMesh()->SetRelativeLocation(FVector(0, 0, -88));
	GetMesh()->SetRelativeRotation(FRotator(0, -90, 0));
	GetMesh()->CustomDepthStencilValue = 10.0f;

	TSubclassOf<UAnimInstance> animClass;
	CHelpers::GetClass<UAnimInstance>(&animClass, "AnimBlueprint'/Game/Enemy/ABP_Enemy.ABP_Enemy_C'");
	GetMesh()->SetAnimInstanceClass(animClass);

	GetCharacterMovement()->MaxWalkSpeed = StatusComp->GetSprintSpeed();
	GetCharacterMovement()->RotationRate = FRotator(0, 720, 0);


	// 적 캐릭터 피격시 보여줄 HP 위젯 생성
	TSubclassOf<UCUserWidget_EnemyHealth> healthClass;
	CHelpers::GetClass<UCUserWidget_EnemyHealth>(&healthClass, "WidgetBlueprint'/Game/Widget/WB_EnemyHealh.WB_EnemyHealh_C'");
	HealthWidget->SetWidgetClass(healthClass);
	HealthWidget->SetRelativeLocation(FVector(0, 0, 190));
	HealthWidget->SetDrawSize(FVector2D(120, 20));
	HealthWidget->SetWidgetSpace(EWidgetSpace::Screen);
}


void ACEnemy::BeginPlay()
{
	// 상태 변화시 호출 할 함수 바인딩
	StateComp->OnStateTypeChanged.AddDynamic(this, &ACEnemy::OnStateTypeChanged);

	Super::BeginPlay();


	// 체력위젯 초기화
	HealthWidget->InitWidget();
	Cast<UCUserWidget_EnemyHealth>(HealthWidget->GetUserWidgetObject())->Update(StatusComp->GetHealth(), StatusComp->GetMaxHealth());
	HiddenHpBar();
}


float ACEnemy::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float damage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	// 방어력 계산
	damage *= (1.0f - (StatusComp->GetDef()/100.0f) );
	DamageInstigator = EventInstigator;

	ActionComp->AbortByDamage();

	// 체력 감소
	StatusComp->SubHealth(damage);
	//CLog::Print("Enemy : " + DamageInstigator->GetName());

	Cast<UCUserWidget_EnemyHealth>(HealthWidget->GetUserWidgetObject())->Update(StatusComp->GetHealth(), StatusComp->GetMaxHealth());

	DamageWidget->UpdateDamage(damage, "");

	if (StatusComp->GetHealth() <= 0.0f)
	{
		StateComp->SetDeadMode();
		return 0.0f;
	}

	ShowHpBar();	// 피격시 5초간 HpBar가 보이게 함

	// 히트리커버리가 높으면 피격모션은 무시 (기본값 : 1)
	if (StatusComp->GetHitRecovery() < 2.0f)	
		StateComp->SetHittedMode();

	return StatusComp->GetHealth();
}


void ACEnemy::Hold(bool InHold)
{
	if (InHold)
	{
		ActionComp->AbortByDamage();
		CustomTimeDilation = 0.0f;
	}
	else
		CustomTimeDilation = 1.0f;


}
void ACEnemy::Hide(bool InHide)
{
	if (InHide)
	{
		GetMesh()->SetVisibility(false);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		GetMesh()->SetVisibility(true);
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

}

void ACEnemy::Hitted()
{
	//SetMove
	StatusComp->SetMove();

	//Play Hitted Montage
	MontagesComp->PlayHitted(StatusComp->GetHitRecovery());

	//Find Look At Rotation
	FVector start = GetActorLocation();
	FVector target = DamageInstigator->GetPawn()->GetActorLocation();
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(start, target));
	//DamageInstigator = nullptr;
}

void ACEnemy::ShowHpBar()
{
	HealthWidget->SetVisibility(true);

	UKismetSystemLibrary::K2_ClearTimer(this, "HiddenHpBar");
	UKismetSystemLibrary::K2_SetTimer(this, "HiddenHpBar", 5.0f, false);
}

void ACEnemy::OnStateTypeChanged(EStateType InPrevType, EStateType InNewType)
{
	switch (InNewType)
	{
	case EStateType::Hitted: Hitted();	break;
	case EStateType::Dead: Dead();	break;
	}
}


void ACEnemy::Dead()
{
	CheckFalse(StateComp->IsDeadMode());

	Cast<UCUserWidget_EnemyHealth>(HealthWidget->GetUserWidgetObject())->Update(0, 100);

	ActionComp->Reset();
	MontagesComp->PlayDead();

	// 맵 외의 충돌 무시
	GetCapsuleComponent()->SetCollisionProfileName("Spectator");

	UKismetSystemLibrary::K2_SetTimer(this, "End_Dead", 5.0f, false);
}

void ACEnemy::End_Dead()
{
	ActionComp->End_Dead();
	Destroy();
}

void ACEnemy::HiddenHpBar()
{
	HealthWidget->SetVisibility(false);
	UKismetSystemLibrary::K2_ClearTimer(this, "HiddenHpBar");
}
