#pragma once

#include "CoreMinimal.h"
#include "Actions/CAction.h"
#include "Components/CActionComponent.h"
#include "CAction_Combo.generated.h"

// �޺� (�÷��̾� �⺻ ����)
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
	void Action() override;			// 1Ÿ ����
	void Begin_Action() override;	// 2Ÿ ����
	void End_Action() override;		// �ൿ ����

	void WeaponAction();		// CWeapon�� ���� ���� �ൿ ����
	void WeaponActionEnd();

	virtual FName GetSocketName() override;

public:
	// CWeapon�� ���ε� �Ұ�
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
