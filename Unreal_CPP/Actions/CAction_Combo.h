#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Combo.generated.h"

// 콤보 (플레이어 기본 공격)
UCLASS()
class U02_CPP_API ACAction_Combo : public ACAction
{
	GENERATED_BODY()

public:
	FORCEINLINE void EnableCombo() { bEnable = true; }
	FORCEINLINE void DisableCombo() { bEnable = false; }

protected:
	virtual void BeginPlay() override;

public:
	void Action() override;			// 1타 실행
	void Begin_Action() override;	// 2타 실행
	void End_Action() override;		// 행동 동료

	void WeaponAction();		// CWeapon에 따른 무기 행동 실행
	void WeaponActionEnd();

	virtual FName GetSocketName() override;

public:
	// CWeapon에 바인딩 할것
	void OnAttachmentBeginOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;
	void OnAttachmentEndOverlap(class ACharacter* InAttacker, class AActor* InAttackCauser, class ACharacter* InOtherCharacter) override;

private:
	UFUNCTION()
		void ResetGlobalDilation();
	UFUNCTION()
		void AbortByTypeChange(EActionType InPrevType, EActionType InNewType);

	virtual void Abort() override;

private:
	int32 Index;
	bool bEnable;
	bool bExist;
	bool bLast;

	TArray<class ACharacter*> HittedCharacters;
};
