#pragma once

#include "CoreMinimal.h"
#include "Objects/CBuildObject.h"
#include "CBuildProjectile.generated.h"

// CAction_WireAction���� ��� �� ����
//	=> CAction_Buildó�� ������ ��ġ ������ ���� (�÷��̾�κ��� �־ ����)
//	=> CAction_Build�� �ٸ��� ��ġ ������ �ϸ� ��ġ �� �������� �� ���Ͱ� ���ư�
UCLASS()
class U02_CPP_API ACBuildProjectile : public ACBuildObject
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UCableComponent* CableComp;

public:
	ACBuildProjectile();
	virtual void BuildSpawn() override;
	virtual void BuildStart(ACharacter* InOwnerCharacter) override;
	virtual void BuildFinish() override;

	virtual void Tick(float DeltaTime) override;

	void SetDirection(FVector start, FVector end, FName InSocketName);

protected:
	virtual void BeginPlay() override;

public:

private:
	bool bCanMove;
	float Rate;
	float Speed;
	FVector StartLocation;
	FVector EndLocation;

	FName SocketName;
};
