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

	// 1. 전방 라인 충돌 (케릭터 기준 전방 +100) => 앞에 벽이 있는지 체크
	FHitResult hitResult;
	if(UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, true, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Red, FLinearColor::Red, 50))
	{
		FVector wallLocation = hitResult.Location;	// 라인충돌의 벽 지점
		WallNormal = hitResult.Normal;				// 라인충돌시 벽의 방향

		FVector center = wallLocation + WallNormal * -10.0f;
		start = center + FVector(0, 0, 200);
		end = center;

		// 2. 상하 라인 충돌 (첫 충돌 지점에서 XY:-10, Z:0~200 사이) => 벽의 높이 체크
		if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Green, FLinearColor::Green, 50))
		{
			FVector wallHeight = hitResult.Location;				// 벽의 높이
			bool bCanClimb = (wallHeight.Z - wallLocation.Z) > 90;	// 벽의 높이 > 100 이면 기어올라감, 아니면 뛰어넘거나 올라 섬

			center = wallLocation + WallNormal * -50.0f;
			start = center + FVector(0, 0, 200);
			end = center + FVector(0, 0, -100);		// 바닥 확인을 위해-

			bool bWallThick = false;

			// 3. 상하 라인 충돌 (첫 충돌 지점에서 XY:-50, Z:-100~200 사이) => 벽의 두께 체크
			if (UKismetSystemLibrary::LineTraceSingleForObjects(GetWorld(), start, end, queries, false, ignores, EDrawDebugTrace::ForDuration, hitResult, true, FLinearColor::Blue, FLinearColor::Blue, 50))
			{
				FVector wallHeight2 = hitResult.Location;			// 충돌 벽이 얇다면 벽 넘어 바닥 지점이 되고, 두꺼우면 벽의 높이가 됨
				bWallThick = (wallHeight.Z - wallHeight2.Z) < 30;	// 높이차가 30보다 작으면 올라가 서야 되고, 크면 뛰어 넘어야 함
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
				// 뛰어 넘는 도중에는 모든 충돌 무시
				OwnerCharacter->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				OwnerCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);
				OwnerCharacter->GetCharacterMovement()->Velocity = FVector(0);
				bMove = true;

				location = OwnerCharacter->GetActorLocation();
				direction = (location - wallLocation).GetSafeNormal();

				// 벽이 두꺼우면 올라섬
				if (bWallThick)
				{
					// 동작 전 위치 조정
					location = wallLocation + direction * 100;
					location.Z = wallHeight.Z;
					OwnerCharacter->SetActorLocation(location);
 
					// 동작 후 위치 설정
					EndLocation = wallLocation + (wallLocation - OwnerCharacter->GetActorLocation()) * 0.75f ;
					EndLocation.Z = OwnerCharacter->GetActorLocation().Z + wallHeight.Z - 44;

					PlayMontage_Common(0);	// 0번 몽타쥬는 올라서는 모션
				}
				// 벽이 얇으면 뛰어 넘음
				else
				{
					// 동작 전 위치 조정
					location = wallLocation + direction * 75;
					location.Z = wallHeight.Z - 20;
					OwnerCharacter->SetActorLocation(location);

					// 동작 후 위치 설정
					EndLocation = wallLocation * 2 - OwnerCharacter->GetActorLocation();


					PlayMontage_Common(1);	// 1번 몽타쥬는 뛰어 넘는 모션
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