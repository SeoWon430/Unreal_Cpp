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
		uint8 TeamID = 0;		// �� ID���� (IGenericTeamAgentInterface �������̽� ���)


private: // SceneComponent
	UPROPERTY(VisibleDefaultsOnly)
		class USpringArmComponent* SpringArm;

	UPROPERTY(VisibleDefaultsOnly)
		class UCameraComponent* Camera;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneCaptureComponent2D* Capture;	// ĳ���� ĸ�� => ������ ȭ�� ���� ȿ��

	UPROPERTY(VisibleDefaultsOnly)
		class UPostProcessComponent* PostProcess;	// ����Ʈ���μ��� => ���, ��ĵ ȿ��

private: // ActorComponent
	UPROPERTY(VisibleDefaultsOnly)
		class UCActionComponent* ActionComp;	// Ű �Է¿� ���� �ൿ ó��(��ų, ��Ÿ, �޽�, ��)

	UPROPERTY(VisibleDefaultsOnly)
		class UCStatusComponent* StatusComp;	// ĳ������ ���� ó��(ü��, ���ݷ� ��)

	UPROPERTY(VisibleDefaultsOnly)
		class UCOptionComponent* OptionComp;	// �ɼ� �� ó��(ī�޶� ȸ�� �ӵ�)

	UPROPERTY(VisibleDefaultsOnly)
		class UCWidgetPlayerComponent* WidgetComp;	// ����(UI) ó��

	UPROPERTY(VisibleDefaultsOnly)
		class UCStateComponent* StateComp;		// ĳ������ ���� 

	UPROPERTY(VisibleDefaultsOnly)
		class UCMontagesComponent* MontagesComp;	// ĳ������ ��Ÿ�� �ִ� ó��



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
	// ��Ÿ�� ����
	FVector WallNormal;
	FVector WallOnForward;
	FVector WallOnRight;
	float RMoveDirection;
	bool bCanWall;
	bool bWallCheck;
	bool bOnWall;		
	bool bWallNone;

	// �ǰݽ� ������
	class AController* DamageInstigator;

	// Ÿ�ӵ����̼�
	float DilationTime = 1.0f;

	// Scan
	FTimeline Timeline;
	class UCurveFloat* Curve;
	class UMaterialParameterCollection* ParameterCollection;
};
