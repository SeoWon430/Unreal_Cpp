#include "CAnimInstance.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"


void UCAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();

	ACharacter* character = Cast<ACharacter>(TryGetPawnOwner());
	CheckNull(character);

	UCActionComponent* action = CHelpers::GetComponent<UCActionComponent>(character);
	CheckNull(action);

	action->OnActionTypeChanged.AddDynamic(this, &UCAnimInstance::OnActionTypeChanged);
}


void UCAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ACharacter* character = Cast<ACharacter>(TryGetPawnOwner());
	CheckNull(character);

	// 캐릭터 움직이는 속도
	Speed = character->GetVelocity().Size();

	// 캐릭터 공중인지 판단
	bInAir = character->GetCharacterMovement()->IsFalling();

	// Direction : 캐릭터 전방 방향 (캐릭터가 벽을 탈 경우 축 방향이 바뀌는것도 계산)
	{
		//FRotator rotate = character->GetControlRotation();
		//Direction = CalculateDirection(character->GetVelocity(), rotate);
		FVector velocity = character->GetVelocity().GetSafeNormal();
		FVector forward = character->GetActorForwardVector();
		float dot = UKismetMathLibrary::Dot_VectorVector(forward, velocity);
		FVector corss = UKismetMathLibrary::Cross_VectorVector(forward, velocity);
		dot -= 1;
		if (forward.Z > corss.Z)
			dot *= 90;
		else
			dot *= -90;

		Direction = dot;
	}
}

void UCAnimInstance::OnActionTypeChanged(EActionType InPrevType, EActionType InNewType)
{
	ActionType = InNewType;
}