#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CStateComponent.generated.h"

// 상태 관리
UENUM(BlueprintType)
enum class EStateType : uint8
{
	Idle, Jump, Turn, Action, Hitted, Dead, Max
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStateTypeChanged, EStateType, InPrevType, EStateType, InNewType);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class U02_CPP_API UCStateComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	FORCEINLINE EStateType GetType() { return Type; }

public:
	UCStateComponent();

public:
	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsIdleMode() { return Type == EStateType::Idle; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsJumpMode() { return Type == EStateType::Jump; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsTurnMode() { return Type == EStateType::Turn; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsActionMode() { return Type == EStateType::Action; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsHittedMode() { return Type == EStateType::Hitted; }

	UFUNCTION(BlueprintPure)
		FORCEINLINE bool IsDeadMode() { return Type == EStateType::Dead; }

public:
	void SetIdleMode();
	void SetJumpMode();
	void SetTurnMode();
	void SetActionMode();
	void SetHittedMode();
	void SetDeadMode();

private:
	void ChangeType(EStateType InNewType);

public:
	//Purpose : 캐릭터 상태 변경에 따른 연쇄 이벤트 발생 용도
	//Call : Set(EStateType)Mode 호출 시
	//Bind : ex) ACPlayer::OnStateTypeChanged -> PlayRollMontage
	UPROPERTY(BlueprintAssignable)
		FStateTypeChanged OnStateTypeChanged;


protected:
	virtual void BeginPlay() override;

private:
	EStateType Type;

};
