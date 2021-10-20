#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CActionComponent.generated.h"

// ���� Ÿ�� (����, Ȱ(�̱���))
//	=> �� ���� Ÿ�Ժ��� �Ʒ� Action���� ����
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Melee, Bow
};

// ��� Action (�Ϲ� + ��Ÿ +��ų)
UENUM(BlueprintType)
enum class EActionType : uint8
{
	// �޽�, �����, ��������, ���̾�, ������, ��Ÿ, ��ų1~4
	Rest, Build, Climb, Wire, Throw, Attack, Skill1, Skill2, Skill3, Skill4, Skill5, None, Max
};

// �Ϲ� Action 
UENUM(BlueprintType)
enum class EActionType_Common : uint8
{
	Rest, Build, Climb, Wire, Throw, Max
};

// ��Ÿ �� ��ų Action
UENUM(BlueprintType)
enum class EActionType_Skill : uint8
{
	Attack, Skill1, Skill2, Skill3, Skill4, Skill5, Max
};

// Action ����� ó��
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
	class ACAction* GetAction();		// ���� ���� ���� Action
	class ACWeapon* GetWeapon();		// ���� ��� �ִ� ����
	class ACCameraActor* GetCameraActor();
	void AttachWeapon(FName InSocketName = "");		// ���⸦ Ư�� SK�� ������ ���̱�

	void Dead();
	void End_Dead();

	// Action ���� �ǰݽ� ȣ��
	void AbortByDamage();

	// ��ų1~4�� ��Ÿ��
	TArray<float> CoolTimes();		

	// Ư�� ��ų�� ī�޶� ����
	void StartCameraMove(FString InSplineName, float InSpeed, float InDelay, ACharacter* InTarget, FName InTargetSocket);

private:
	void ChangeType(EActionType InNewType);
	void Action(EActionType InType);			// �Է¿� ���� Action ����

protected:
	virtual void BeginPlay() override;

public:
	//Purpose : ���⿡ ���� ���� ���� ����
	//Call : Set(EActionType)Mode ȣ�� ��
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


	// DataAsset�� ���� �� Action���� ������(��Ÿ��, ���ݷ�, ��) ó��

	// 1. �Ϲ� Action
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsCommon[(int32)EActionType_Common::Max];		// ���ÿ� ActionData
	UPROPERTY()
		class UCActionData* DatasCommon[(int32)EActionType_Common::Max];			// ó���� ActionData

	// 2. ����(����) Action => ��Ÿ&��ų1~4
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsMelee[(int32)EActionType_Skill::Max];
	UPROPERTY()
		class UCActionData* DatasMelee[(int32)EActionType_Skill::Max];

	// 3. ����(Ȱ) Action => ��Ÿ&��ų1~4	(�̱���)
	UPROPERTY(EditDefaultsOnly, Category = "ActionData")
		class UCActionManager* DataAssetsBow[(int32)EActionType_Skill::Max];
	UPROPERTY()
		class UCActionData* DatasBow[(int32)EActionType_Skill::Max];

		
};
