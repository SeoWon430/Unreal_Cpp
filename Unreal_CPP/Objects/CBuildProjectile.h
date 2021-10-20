#pragma once

#include "CoreMinimal.h"
#include "Objects/CBuildObject.h"
#include "CBuildProjectile.generated.h"

// CAction_WireAction에서 사용 될 액터
//	=> CAction_Build처럼 벽에만 설치 가능한 액터 (플레이어로부터 멀어도 가능)
//	=> CAction_Build와 다르게 설치 결정을 하면 설치 한 방향으로 이 액터가 날아감
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
