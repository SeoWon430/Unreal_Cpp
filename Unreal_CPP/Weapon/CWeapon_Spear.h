
#pragma once

#include "CoreMinimal.h"
#include "Weapon/CWeapon.h"
#include "CWeapon_Spear.generated.h"

USTRUCT(BlueprintType)
struct FSpearData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		float Speed;

	UPROPERTY(EditAnywhere)
		float Range;

	UPROPERTY(EditAnywhere)
		float Duration;

	UPROPERTY(EditAnywhere)
		float CollisionRadius;

	UPROPERTY(EditAnywhere)
		FName StartSocketName;

	UPROPERTY(EditAnywhere)
		FName EndAttachSocketName;
};

UCLASS()
class U02_CPP_API ACWeapon_Spear : public ACWeapon
{
	GENERATED_BODY()
	
public:
	ACWeapon_Spear();
	void Action(UINT InAttackIndex) override;
	void ActionEnd() override;
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
		TArray<FSpearData> AttackData;
	
private:
	FVector Direction;

	bool bAction;
	bool bMove;
	UINT Index;

	float CurrentRange;
	float CurrentDuration;
	
};
