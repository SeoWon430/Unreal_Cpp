#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CStatusComponent.generated.h"

// �������ͽ� ����
UENUM(BlueprintType)
enum class ECharacterSpeed : uint8
{
	Walk, Run, Sprint, Max
};

USTRUCT(BlueprintType)
struct FStatus
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float Atk = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float Def = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float HitRecovery = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float MotionSpeed = 1.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float MaxHealth = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Status")
		float MaxStamina = 500.0f;


};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class U02_CPP_API UCStatusComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere, Category = "Status")
		FStatus OriginStatus;					// �⺻ ����

	UPROPERTY(EditAnywhere, Category = "Status")
		float MoveSpeed[(int32)ECharacterSpeed::Max] = { 200, 400, 700 };

public:
	FORCEINLINE float GetHealth() { return Health; }
	FORCEINLINE float GetStamina() { return Stamina; }
	FORCEINLINE bool CanMove() { return bCanMove; }
	FORCEINLINE bool GetHitMotion() { return HitMotion; }

	// ������ Get�Ҷ��� �⺻(Origin)���Ȱ� �߰�(Add)������ ���Ͽ� return
	float GetMaxHealth();
	float GetHealthRatio();

	float GetMaxStamina();
	float GetStaminaRatio();

	float GetAtk();
	float GetDef();
	float GetHitRecovery();
	float GetMotionSpeed();

	float GetWalkSpeed();
	float GetRunSpeed();
	float GetSprintSpeed();


	// ���� ����
	void AddHealth(float InAmount);
	void SubHealth(float InAmount);

	void AddStamina(float InAmount);
	void SubStamina(float InAmount);

	void AddAtk(float InAmount);
	void AddDef(float InAmount);
	void AddHitRecovery(float InSpeed);
	void AddMotionSpeed(float InSpeed);

	void SetSpeed(ECharacterSpeed InSpeed);

	void SetMove();
	void SetStop();
	void SetHitMotion(bool InMotion);

	// ���� ����ü ����
	FORCEINLINE FStatus GetOriginStatus() { return OriginStatus; }
	FORCEINLINE FStatus GetAddStatus() { return AddStatus; }

public:
	UCStatusComponent();

protected:
	virtual void BeginPlay() override;

private:
	ECharacterSpeed CurrentMove;
	FStatus AddStatus;			// �߰� ����

	float Health;	// ���� Hp
	float Stamina;	// ���� Mp

	bool HitMotion = true;
	bool bCanMove = true;	// Action�� �̵� ����
};
