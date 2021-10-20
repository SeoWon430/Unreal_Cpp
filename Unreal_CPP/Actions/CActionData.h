#pragma once
#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "CActionData.generated.h"


// Action에서 사용 될 데이터
// 아래 구조체는 DataAsset으로 관리 => CActionManager.cpp
USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		class UAnimMontage* AnimMontage;	// Action 몽타쥬

	UPROPERTY(EditAnywhere)
		float PlayRate = 1.0f;				// Action 몽타쥬 재생 속도

	UPROPERTY(EditAnywhere)
		FName StartSection;					// Action 몽타쥬 시작점

	UPROPERTY(EditAnywhere)
		bool bCanMove = true;				// Action 중 움직임 가능 여부 => StatusComp

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
		float BuildRange = 200.0f;		// Action_Build에서 설치 가능 거리

	UPROPERTY(EditAnywhere)				// Action_Build에서 설치 가능 축 설정 (Z면 기본 캐릭터가 서있는 평면)
		bool BuildDotX = false;
	UPROPERTY(EditAnywhere)
		bool BuildDotY = false;
	UPROPERTY(EditAnywhere)
		bool BuildDotZ = true;

	UPROPERTY(EditAnywhere)
		float BuildDot = 0.5f;		// Action_Build에서 설치 가능 경사도

	UPROPERTY(EditAnywhere)
		FName SpawnSocket = "None";

	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACBuildObject> BuildObjectClass;	// Action_Build에서 설치 할 오브젝트
};	


// Action의 공격용 데이터
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
	//	TSubclassOf<class ACDamageBox> DamageBoxClass;	// 충돌처리용(테스트)

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCameraShake> ShakeClass;

	UPROPERTY(EditAnywhere)
		FName SocketName;

	UPROPERTY(EditAnywhere)
		FName CameraPathName;			// 카메라 무빙 사용시 적용될 Spline경로 이름 (Spline은 Player의 BP에 만들것)

	UPROPERTY(EditAnywhere)
		FName CameraTargetSocketName;	// 카메라 무빙 시 바라볼 대상의 소켓 이름

	UPROPERTY(EditAnywhere)
		float CameraMoveSpeed = 0.0f;	// 카메라 무빙 속도

	UPROPERTY(EditAnywhere)
		float CameraMoveStopTime = 0.0f;	// 카메라 Spline최종지점 도착시 일정시간 유지(이후 시점은 원상태로 돌아옴)
};



UCLASS()
class U02_CPP_API UCActionData : public UObject
{
	GENERATED_BODY()


public:
	friend class UCActionManager;	// 여기 있는 private도 UCActionData에서 접근 가능하게 해줌

public:
	FORCEINLINE class ACAction* GetAction() { return Action; }

private:
	class ACAction* Action;

};
