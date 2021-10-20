#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CActionData.generated.h"


// Action���� ��� �� ������
// �Ʒ� ����ü�� DataAsset���� ���� => CActionManager.cpp
USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		class UAnimMontage* AnimMontage;	// Action ��Ÿ��

	UPROPERTY(EditAnywhere)
		float PlayRate = 1.0f;				// Action ��Ÿ�� ��� �ӵ�

	UPROPERTY(EditAnywhere)
		FName StartSection;					// Action ��Ÿ�� ������

	UPROPERTY(EditAnywhere)
		bool bCanMove = true;				// Action �� ������ ���� ���� => StatusComp

};

USTRUCT(BlueprintType)
struct FActionData_Common : public FActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		float RecoverHp;		

	UPROPERTY(EditAnywhere)
		float RecoverMp;	

	UPROPERTY(EditAnywhere)
		float BuildRange = 200.0f;		// Action_Build���� ��ġ ���� �Ÿ�

	UPROPERTY(EditAnywhere)				// Action_Build���� ��ġ ���� �� ���� (Z�� �⺻ ĳ���Ͱ� ���ִ� ���)
		bool BuildDotX = false;
	UPROPERTY(EditAnywhere)
		bool BuildDotY = false;
	UPROPERTY(EditAnywhere)
		bool BuildDotZ = true;

	UPROPERTY(EditAnywhere)
		float BuildDot = 0.5f;		// Action_Build���� ��ġ ���� ��絵

	UPROPERTY(EditAnywhere)
		FName SpawnSocket = "None";

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACBuildObject> BuildObjectClass;	// Action_Build���� ��ġ �� ������Ʈ
};	


// Action�� ���ݿ� ������
USTRUCT(BlueprintType)
struct FActionData_Attack : public FActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		float Power = 5.0f;			// Damage

	UPROPERTY(EditAnywhere)
		float LaunchPower = 0.0f;

	UPROPERTY(EditAnywhere)
		float CoolTime = 0.0f;

	UPROPERTY(EditAnywhere)
		float Stamina = 0.0f;		// Mp

	UPROPERTY(EditAnywhere)
		float TimeDilation = 1.0f;

	UPROPERTY(EditAnywhere)
		class UParticleSystem* Effect;

	UPROPERTY(EditAnywhere)
		FTransform EffectTransform;

	//UPROPERTY(EditAnywhere)
	//	TSubclassOf<class ACDamageBox> DamageBoxClass;	// �浹ó����(�׽�Ʈ)

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCameraShake> ShakeClass;

	UPROPERTY(EditAnywhere)
		FName SocketName;

	UPROPERTY(EditAnywhere)
		FName CameraPathName;			// ī�޶� ���� ���� ����� Spline��� �̸� (Spline�� Player�� BP�� �����)

	UPROPERTY(EditAnywhere)
		FName CameraTargetSocketName;	// ī�޶� ���� �� �ٶ� ����� ���� �̸�

	UPROPERTY(EditAnywhere)
		float CameraMoveSpeed = 0.0f;	// ī�޶� ���� �ӵ�

	UPROPERTY(EditAnywhere)
		float CameraMoveStopTime = 0.0f;	// ī�޶� Spline�������� ������ �����ð� ����(���� ������ �����·� ���ƿ�)
};



UCLASS()
class U02_CPP_API UCActionData : public UObject
{
	GENERATED_BODY()


public:
	friend class UCActionManager;	// ���� �ִ� private�� UCActionData���� ���� �����ϰ� ����

public:
	FORCEINLINE class ACAction* GetAction() { return Action; }

private:
	class ACAction* Action;

};
