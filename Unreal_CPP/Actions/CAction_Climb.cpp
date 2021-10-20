#include "CAction_Climb.h"
#include "Global.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/CStateComponent.h"
#include "Components/CStatusComponent.h"
#include "Character/CPlayer.h"


ACAction_Climb::ACAction_Climb()
	:ACAction()
{

}


void ACAction_Climb::BeginPlay()
{
	Super::BeginPlay();

	ActionComp->OnActionTypeChanged.AddDynamic(this, &ACAction_Climb::AbortByTypeChange);
}


void ACAction_Climb::Action()
{
	CheckFalse(StateComp->IsIdleMode());

	FVector start = OwnerCharacter->GetActorLocation() - FVector(0, 0, 40);
	FVector end = start + OwnerCharacter->GetActorForwardVector() * 150.0f;

	TArray<TEnumAsByte<EObjectTypeQuery>> queries;
	queries.Add(EObjectTypeQuery::ObjectTypeQuery1);
	TArray<AActor*> ignores;
	ignores.Add(this);

	// 1. ���� ���� �浹 (�ɸ��� ���� ���� +100) => �տ� ���� �ִ��� üũ
	FHitResult hitResult;
	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Red, FLinearColor::Red, 50))
	{
		FVector wallLocation = hitResult.Location;	// �����浹�� �� ����
		WallNormal = hitResult.Normal;				// �����浹�� ���� ����

		FVector center = wallLocation + WallNormal * -10.0f;
		start = center + FVector(0, 0, 200);
		end = center;

		// 2. ���� ���� �浹 (ù �浹 �������� XY:-10, Z:0~200 ����) => ���� ���� üũ
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Green, FLinearColor::Green, 50))
		{
			FVector wallHeight = hitResult.Location;				// ���� ����
			bool bCanClimb = (wallHeight.Z - wallLocation.Z) > 90;	// ���� ���� > 100 �̸� ���ö�, �ƴϸ� �پ�Ѱų� �ö� ��

			center = wallLocation + WallNormal * -50.0f;
			start = center + FVector(0, 0, 200);
			end = center + FVector(0, 0, -100);		// �ٴ� Ȯ���� ����-

			bool bWallThick = false;

			// 3. ���� ���� �浹 (ù �浹 �������� XY:-50, Z:-100~200 ����) => ���� �β� üũ
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Blue, FLinearColor::Blue, 50))
			{
				FVector wallHeight2 = hitResult.Location;			// �浹 ���� ��ٸ� �� �Ѿ� �ٴ� ������ �ǰ�, �β���� ���� ���̰� ��
				bWallThick = (wallHeight.Z - wallHeight2.Z) < 30;	// �������� 30���� ������ �ö� ���� �ǰ�, ũ�� �پ� �Ѿ�� ��
			}


			FVector location;
			FVector direction;
			if (bCanClimb)
			{
				start = OwnerCharacter->GetActorLocation() + FVector(0, 0, 200);
				end = start + OwnerCharacter->GetActorForwardVector() * 70.0f;
				FVector start2 = OwnerCharacter->GetActorLocation();
				FVector end2 = start + FVector(0, 0, 200);

				
				FHitResult hitResult4;
				FHitResult hitResult5;
				if (!UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult4, true, FLinearColor::Yellow, FLinearColor::Yellow, 50)
					&& UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start2, end2, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult5, true, FLinearColor::Black, FLinearColor::Black, 50))

				{
					OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
					OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
					OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0);

					location = OwnerCharacter->GetActorLocation();
					direction = UKismetMathLibrary::GetForwardVector(UKismetMathLibrary::MakeRotFromX(WallNormal));
					location += direction * 30;
					EndLocation = location;
					EndLocation.Z += wallHeight.Z - 44;
					OwnerCharacter->SetActorLocation(location);

					PlayMontage_Common(2);
				}
				
			}
			else
			{
				// �پ� �Ѵ� ���߿��� ��� �浹 ����
				OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0);
				bMove = true;

				location = OwnerCharacter->GetActorLocation();
				direction = (location - wallLocation).GetSafeNormal();

				// ���� �β���� �ö�
				if (bWallThick)
				{
					// ���� �� ��ġ ����
					location = wallLocation + direction * 100;
					location.Z = wallHeight.Z;
					OwnerCharacter->SetActorLocation(location);
 
					// ���� �� ��ġ ����
					EndLocation = wallLocation + (wallLocation - OwnerCharacter->GetActorLocation()) * 0.75f ;
					EndLocation.Z = OwnerCharacter->GetActorLocation().Z + wallHeight.Z - 44;

					PlayMontage_Common(0);	// 0�� ��Ÿ��� �ö󼭴� ���
				}
				// ���� ������ �پ� ����
				else
				{
					// ���� �� ��ġ ����
					location = wallLocation + direction * 75;
					location.Z = wallHeight.Z - 20;
					OwnerCharacter->SetActorLocation(location);

					// ���� �� ��ġ ����
					EndLocation = wallLocation * 2 - OwnerCharacter->GetActorLocation();


					PlayMontage_Common(1);	// 1�� ��Ÿ��� �پ� �Ѵ� ���
				}
			}
		}
	}
}


void ACAction_Climb::Begin_Action()
{

}


void ACAction_Climb::End_Action()
{
	Super::End_Action();

	OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	OwnerCharacter->SetActorLocation(EndLocation);
	bMove = false;
}


void ACAction_Climb::AbortByTypeChange(EActionType InPrevType, EActionType InNewType)
{
	if (InPrevType == EActionType::Build && InPrevType != InNewType)
		End_Action();
}


void ACAction_Climb::Abort()
{
	End_Action();
}


void ACAction_Climb::Tick(float DeltaTime)
{
	if (bMove)
		OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0, 0, 0);
}