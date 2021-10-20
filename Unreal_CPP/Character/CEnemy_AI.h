
#pragma once

#include "CoreMinimal.h"
#include "Character/CEnemy.h"
#include "CEnemy_AI.generated.h"

UCLASS()
class U02_CPP_API ACEnemy_AI : public ACEnemy
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "AI")
		class UBehaviorTree* BehaviorTree;		// 행동 방식을 정의할 BT

	UPROPERTY(EditDefaultsOnly, Category = "AI")
		uint8 TeamID = 1;

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UCPatrolComponent* Patrol;	// BT에서 처리할 순찰 경로(Path)

public:
	FORCEINLINE class UBehaviorTree* GetBehaviorTree() { return BehaviorTree; }
	FORCEINLINE uint8 GetTeamID() { return TeamID; }

public:
	ACEnemy_AI();
};
