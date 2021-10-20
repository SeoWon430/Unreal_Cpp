#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/CStateComponent.h"
#include "CEnemy.generated.h"

UCLASS()
class U02_CPP_API ACEnemy : public ACharacter
{
	GENERATED_BODY()

private: //SceneComponent
	UPROPERTY(VisibleDefaultsOnly)
		class UWidgetComponent* HealthWidget;	// ĳ���� ü��UI

	UPROPERTY(VisibleDefaultsOnly)
		class UCWidgetDamageTextComponent* DamageWidget;	// �ǰݽ� ������ ��ġ ǥ��

protected: //ActorCompnent
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly)
		class UCActionComponent* ActionComp;

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UCMontagesComponent* MontagesComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UCStatusComponent* StatusComp;

	UPROPERTY(VisibleDefaultsOnly)
		class UCStateComponent* StateComp;

public:
	ACEnemy();

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	FORCEINLINE bool IsDead() { return StateComp->IsDeadMode(); }
	
	void Hold(bool InHold);
	void Hide(bool InHide);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void OnStateTypeChanged(EStateType InPrevType, EStateType InNewType);

private:
	void Hitted();
	void Dead();
	void ShowHpBar();

	UFUNCTION()
		void End_Dead();

	UFUNCTION()
		void HiddenHpBar();

private:
	class AController* DamageInstigator;
};
