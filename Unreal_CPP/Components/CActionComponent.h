#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CActionComponent.generated.h"

// 무기 타입 (근접, 활(미구현))
//	=> 각 무기 타입별로 아래 Action들을 가짐
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Melee, Bow
};

// 모든 Action (일반 + 평타 +스킬)
UENUM(BlueprintType)
enum class EActionType : uint8
{
	// 휴식, 만들기, 벽오르기, 와이어, 던지기, 평타, 스킬1~4
	Rest, Build, Climb, Wire, Throw, Attack, Skill1, Skill2, Skill3, Skill4, Skill5, None, Max
};

// 일반 Action 
UENUM(BlueprintType)
enum class EActionType_Common : uint8
{
	Rest, Build, Climb, Wire, Throw, Max
};

// 평타 및 스킬 Action
UENUM(BlueprintType)
enum class EActionType_Skill : uint8
{
	Attack, Skill1, Skill2, Skill3, Skill4, Skill5, Max
};

// Action 변경시 처리
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActionTypeChanged, EActionType, InPrevType, EActionType, InNewType);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class U02_CPP_API UCActionComponent : public UActorComponent
{
	GENERATED_BODY()
private:
	UPROPERTY(EditAnywhere)
		bool bUseCameraAction = false;

public:
	UCActionComponent();

public:
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsNoneMode() { return ActionType == EActionType::None; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsRestMode() { return ActionType == EActionType::Rest; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsBuildMode() { return ActionType == EActionType::Build; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsThrowMode() { return ActionType == EActionType::Throw; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsAttackMode() { return ActionType == EActionType::Attack; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsWireMode() { return ActionType == EActionType::Wire; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsSkill1Mode() { return ActionType == EActionType::Skill1; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsSkill2Mode() { return ActionType == EActionType::Skill2; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsSkill3Mode() { return ActionType == EActionType::Skill3; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsSkill4Mode() { return ActionType == EActionType::Skill4; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsSkill5Mode() { return ActionType == EActionType::Skill5; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsMeleeMode() { return WeaponType == EWeaponType::Melee; }
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsBowMode() { return WeaponType == EWeaponType::Bow; }


	FORCEINLINE EActionType GetActionMode() { return ActionType; }


public:
	UFUNCTION(BlueprintCallable) void Action_Rest();
	UFUNCTION(BlueprintCallable) void Action_Build();
	UFUNCTION(BlueprintCallable) void Action_Climb();
	UFUNCTION(BlueprintCallable) void Action_Wire();
	UFUNCTION(BlueprintCallable) void Action_Throw();
	UFUNCTION(BlueprintCallable) void Action_Attack();
	UFUNCTION(BlueprintCallable) void Action_Skill1();
	UFUNCTION(BlueprintCallable) void Action_Skill2();
	UFUNCTION(BlueprintCallable) void Action_Skill3();
	UFUNCTION(BlueprintCallable) void Action_Skill4();
	UFUNCTION(BlueprintCallable) void Action_Skill5();
	UFUNCTION(BlueprintCallable) void UnAction_Skill4();


public:
	void Action();
	void Reset();
	class ACAction* GetAction();		// 현재 실행 중인 Action
	class ACWeapon* GetWeapon();		// 현재 들고 있는 무기
	class ACCameraActor* GetCameraActor();
	void AttachWeapon(FName InSocketName = "");		// 무기를 특정 SK의 소켓이 붙이기

	void Dead();
	void End_Dead();

	// Action 도중 피격시 호출
	void AbortByDamage();

	// 스킬1~4의 쿨타임
	TArray<float> CoolTimes();		

	// 특정 스킬의 카메라 무브
	void StartCameraMove(FString InSplineName, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket);

private:
	void ChangeType(EActionType InNewType);
	void Action(EActionType InType);			// 입력에 따른 Action 실행

protected:
	virtual void BeginPlay() override;

public:
	//Purpose : 무기에 따른 블렌드 포즈 변경
	//Call : Set(EActionType)Mode 호출 시
	//Bind : ex) ACAnimInstance::OnTypeChanged
	UPROPERTY(BlueprintAssignable)
		FActionTypeChanged OnActionTypeChanged;

private:
	EWeaponType WeaponType;
	EActionType ActionType;
	class ACAction* CurrentAction;
	class ACWeapon* Weapon;

	class ACCameraActor* CameraActor;
	TArray<class USplineComponent*> CameraSplines;

	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		TSubclassOf<class ACWeapon> WeaponClass;


	// DataAsset을 통한 각 Action들의 데이터(몽타쥬, 공격력, 등) 처리

	// 1. 일반 Action
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsCommon[(int32)EActionType_Common::Max];		// 세팅용 ActionData
	UPROPERTY()
		class UCActionData* DatasCommon[(int32)EActionType_Common::Max];			// 처리용 ActionData

	// 2. 무기(근접) Action => 평타&스킬1~4
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsMelee[(int32)EActionType_Skill::Max];
	UPROPERTY()
		class UCActionData* DatasMelee[(int32)EActionType_Skill::Max];

	// 3. 무기(활) Action => 평타&스킬1~4	(미구현)
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsBow[(int32)EActionType_Skill::Max];
	UPROPERTY()
		class UCActionData* DatasBow[(int32)EActionType_Skill::Max];

		
};
