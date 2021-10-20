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

	// ĳ���� �����̴� �ӵ�
	Speed = character->GetVelocity().Size();

	// ĳ���� �������� �Ǵ�
	bInAir = character->GetCharacterMovement()->IsFalling();

	// Direction : ĳ���� ���� ���� (ĳ���Ͱ� ���� Ż ��� �� ������ �ٲ�°͵� ���)
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