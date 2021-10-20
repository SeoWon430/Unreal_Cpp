#include "CLaserCutter.h"
#include "Global.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Character/CEnemy.h"
#include "Character/CPlayer.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProceduralMeshComponent.h"
#include "KismetProceduralMeshLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Objects/CProceduralSkeletalMesh.h"

ACLaserCutter::ACLaserCutter()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent<USphereComponent>(this, &Sphere, "Sphere");
	CHelpers::GetAsset<UNiagaraSystem>(&LaserNiagara, "NiagaraSystem'/Game/Niagara/Laser/NS_LaserCutter.NS_LaserCutter'");
	CHelpers::CreateActorComponent<UProjectileMovementComponent>(this, &Projectile, "Projectile");

	Sphere->SetSphereRadius(5.0f);
	Sphere->SetSimulatePhysics(true);
	Sphere->SetEnableGravity(false);
	Sphere->SetNotifyRigidBodyCollision(true);
	Sphere->BodyInstance.SetCollisionProfileName("BlockAllDynamic");

	Projectile->InitialSpeed = 0;
	Projectile->MaxSpeed = 0;
	Projectile->ProjectileGravityScale = 0.0f;
}

void ACLaserCutter::BeginPlay()
{
	Super::BeginPlay();
	MakeLaser();
}

void ACLaserCutter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTrue(bHide);

	// 타격하고 일정시간 타격 무시
	if (bHit)
	{
		hitTime += DeltaTime;
		if(hitTime > 0.25f)
			bHit = false;
	}

	
	//CheckFalse(bLaser);
	if (CurrentIndex > (UINT)ReflectCount)
	{
		bLaser = false;
		Projectile->InitialSpeed = 0.0f;
		Projectile->MaxSpeed = 0.0f;
		Projectile->StopMovementImmediately();
		return;
	}
	
	SetLaser(CurrentIndex, 1, 0, StartLocation, GetActorLocation(), GetActorForwardVector());
}



void ACLaserCutter::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	CheckTrue(bHit);
	CheckNull(SweepResult.GetActor());
	CheckTrue(OtherActor == GetOwner());
	CheckTrue(OtherActor->GetName().Contains("Laser"));

	CheckFalse(Cast<ACPlayer>(OtherActor) == nullptr);

	ACProceduralSkeletalMesh* procMesh = Cast<ACProceduralSkeletalMesh>(OtherActor);
	if (procMesh != nullptr)
	{
		if (bCut == false)
		{
			procMesh->Slice(SweepResult, GetActorUpVector());
			bCut = true;
			//UKismetSystemLibrary::K2_SetTimer(this, "Stop", 0.5f, false);
		}
		return;
	}


	bHit = true;
	bCut = false;

	FVector normal = SweepResult.Normal;
	if (UKismetMathLibrary::Dot_VectorVector(normal, GetActorForwardVector()) > 0)
		normal *= -1;
	FVector direction = UKismetMathLibrary::GetReflectionVector(GetActorForwardVector(), normal).GetSafeNormal();


	ChangeDirection(direction);
}




void ACLaserCutter::MakeLaser()
{
	for (int32 i = 0; i < ReflectCount + 1; i++)
	{
		UNiagaraComponent* niagara = UNiagaraFunctionLibrary::SpawnSystemAttached
		(LaserNiagara, Sphere, FName()
			, GetActorLocation(), FRotator()
			, EAttachLocation::KeepRelativeOffset, false);
		Lasers.Add(niagara);
		niagara->bRenderCustomDepth = true;
		niagara->CustomDepthStencilValue = 10.0f;
		niagara->MarkRenderStateDirty();
		SetLaser(i, 0, 0, FVector(0), FVector(0), FVector(0), false);
	}

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &ACLaserCutter::OnComponentBeginOverlap);
	Hide();
}


void ACLaserCutter::ChangeDirection(FVector InDirection)
{
	SetLaser(CurrentIndex, 1, 1, StartLocation, GetActorLocation(), GetActorForwardVector());
	StartLocation = GetActorLocation();

	CurrentIndex++;
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + InDirection));
	Projectile->Velocity = InDirection * Speed;
	//CLog::Print(Projectile->Velocity);

}

void ACLaserCutter::SetLaser(UINT Index, UINT InSpawnProb, UINT InFireProb, FVector InStart, FVector InEnd, FVector InDirection, bool InVisible)
{
	CheckTrue(Index >= (UINT) Lasers.Num());
	Lasers[Index]->SetFloatParameter("LaserSpawnProb", InSpawnProb);		// 레이저의 생성 갯수 (0이면 생성X, 1이면 보임)
	Lasers[Index]->SetFloatParameter("LaserFireSpawnProb", InFireProb);		// 반사되는 면에 생길 불꽃 이펙트 갯수
	Lasers[Index]->SetVectorParameter("LaserStart", InStart);				// 레이저(선분)의 시작 점
	Lasers[Index]->SetVectorParameter("LaserEnd", InEnd);					// 레이저(선분)의 끝 점
	Lasers[Index]->SetVectorParameter("LaserNormal", InDirection);			// 튀는 불꽃 방향을 위한 Normal
	Lasers[Index]->SetVisibility(InVisible);
}

void ACLaserCutter::Start(FVector InStart, FVector InDirection, class ACProceduralSkeletalMesh* InProcMesh, float InDelay)
{
	//ChangeDirection(InDirection);
	CurrentIndex = 0;
	SetActorLocation(InStart);
	SetActorRotation(UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), GetActorLocation() + InDirection));
	Lasers[0]->SetVisibility(true);
	StartLocation = InStart;
	bHide = false;
	SetActorEnableCollision(ECollisionEnabled::QueryOnly);

	if (InProcMesh == nullptr)
	{
		TSubclassOf<ACProceduralSkeletalMesh> procClass = ACProceduralSkeletalMesh::StaticClass();
		TArray<AActor*> actors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), procClass, actors);
		if (actors.Num() > 0)
		{
			Target = Cast<ACProceduralSkeletalMesh>(actors[0]);
		}
	}
	else
	{
		Target = InProcMesh;
	}

	//CLog::Print("Start");
	StartLaser();
	UKismetSystemLibrary::K2_SetTimer(this, "StartLaser", InDelay, false);
}


void ACLaserCutter::StartLaser()
{
	//CLog::Print("Start2");
	bHit = false;
	bCut = false;
	Projectile->Velocity = GetActorForwardVector() * Speed;
	Projectile->InitialSpeed = Speed;
	Projectile->MaxSpeed = Speed*1.5f;
}

void ACLaserCutter::Stop()
{
	Projectile->StopMovementImmediately();
	Projectile->InitialSpeed = 0.0f;
	Projectile->MaxSpeed = 0.0f;
}

void ACLaserCutter::Hide()
{
	bHide = true;
	SetActorEnableCollision(ECollisionEnabled::NoCollision);
	Projectile->StopMovementImmediately();
	Projectile->InitialSpeed = 0.0f;
	Projectile->MaxSpeed = 0.0f;


	for (UINT i = 0; i < (UINT)Lasers.Num(); i++)
	{
		Lasers[i]->SetVisibility(false);
		Lasers[i]->PrimaryComponentTick.bCanEverTick = false;
	}
}

