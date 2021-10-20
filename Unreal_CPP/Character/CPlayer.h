#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "Components/TimelineComponent.h"
#include "GenericTeamAgentInterface.h"
#include "CPlayer.generated.h"

UCLASS()
class U02_CPP_API ACPlayer : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Team")
		uint8 TeamID = 0;		// 팀 ID설정 (IGenericTeamAgentInterface 인터페이스 상속)


private: // SceneComponent
	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneCaptureComponent2D* Capture;	// 캐릭터 캡쳐 => 죽을때 화면 깨짐 효과

	UPROPERTY(VisibleDefaultsOnly)
		class UPostProcessComponent* PostProcess;	// 포스트프로세스 => 흑백, 스캔 효과

private: // ActorComponent
	UPROPERTY(VisibleDefaultsOnly)
		class UCActionComponent* ActionComp;	// 키 입력에 따른 행동 처리(스킬, 평타, 휴식, 등)

	UPROPERTY(VisibleDefaultsOnly)
		class UCStatusComponent* StatusComp;	// 캐릭터의 스탯 처리(체력, 공격력 등)

	UPROPERTY(VisibleDefaultsOnly)
		class UCOptionComponent* OptionComp;	// 옵션 값 처리(카메라 회전 속도)

	UPROPERTY(VisibleDefaultsOnly)
		class UCWidgetPlayerComponent* WidgetComp;	// 위젯(UI) 처리

	UPROPERTY(VisibleDefaultsOnly)
		class UCStateComponent* StateComp;		// 캐릭터의 상태 

	UPROPERTY(VisibleDefaultsOnly)
		class UCMontagesComponent* MontagesComp;	// 캐릭터의 몽타쥬 애님 처리



public:
	ACPlayer();

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;


protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual FGenericTeamId GetGenericTeamId() const;
	FVector CameraDirection();

	void OffJump();

	void SetCameraFar();
	void SetCameraFarReset();

	FORCEINLINE void SetWallOn() { bOnWall = true; }
	FORCEINLINE void SetWallOff() { bOnWall = false; }
	FORCEINLINE void SetWallCheckOn() { bWallCheck = true; }
	FORCEINLINE void SetWallCheckOff() { bWallCheck = false; }
	FORCEINLINE void SetCanWallOn() { bCanWall = true; }
	FORCEINLINE void SetCanWallOff() { bCanWall = false; }
	bool IsWallOn() { return bOnWall; }

private:
	void OnMoveForward(float InAxis);
	void OnMoveRight(float InAxis);
	void OnHorizontalLook(float InAxis);
	void OnVerticalLook(float InAxis);

	void OnAction();
	void OnRest();
	void OnBuild();
	void OnClimb();
	void OnWire();
	void OnThrow();
	void OnWallToggle();


	void OnSkill1();
	void OnSkill2();
	void OnSkill3();

	void OnSkill4();
	void OffSkill4();
	void OnSkill5();

	void OnMenu();
	void OnScan();
	UFUNCTION()
		void OffScan();
	UFUNCTION()
		void OnScanning(float Output);

	void WallCheck();
	void OnWall(FVector InLocation);
	void OnWallMaintain(FVector InLocation);
	void OffWall(FVector InLocation);


	void Hitted();
	void Dead();
	UFUNCTION()
		void End_Dead();


public:
	void CollisionOff();
	void CollisionOn();

	void OnJump();

	void OnBlackOut();
	void OffBlackOut();
	void SetDilationTime(float InRate = 0.1f);
	void UpdateStatusWidget();

private:
	UFUNCTION()
		void OnStateTypeChanged(EStateType InPrevType, EStateType InNewType);


private:
	// 벽타기 관련
	FVector WallNormal;
	FVector WallOnForward;
	FVector WallOnRight;
	float RMoveDirection;
	bool bCanWall;
	bool bWallCheck;
	bool bOnWall;		
	bool bWallNone;

	// 피격시 공격자
	class AController* DamageInstigator;

	// 타임딜레이션
	float DilationTime = 1.0f;

	// Scan
	FTimeline Timeline;
	class UCurveFloat* Curve;
	class UMaterialParameterCollection* ParameterCollection;
};
