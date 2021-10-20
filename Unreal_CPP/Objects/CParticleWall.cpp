
#include "CParticleWall.h"
#include "Global.h"
#include "NiagaraComponent.h"
#include "GameFramework/Character.h"

ACParticleWall::ACParticleWall()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &NiagaraComp, "NiagaraComp");

	UNiagaraSystem* patricle;
	CHelpers::GetAsset<UNiagaraSystem>(&patricle, "NiagaraSystem'/Game/Niagara/Wall/ParticleWall_System.ParticleWall_System'");
	NiagaraComp->SetAsset(patricle);
	NiagaraComp->SetVectorParameter("ParticleCount", ParticleCount);
	NiagaraComp->SetFloatParameter("ParticleSize", ParticleSize);
}

void ACParticleWall::OnConstruction(const FTransform& Transform)
{
	CheckNull(NiagaraComp);
	NiagaraComp->SetVectorParameter("ParticleCount", ParticleCount);
	NiagaraComp->SetFloatParameter("ParticleSize", ParticleSize);
}

void ACParticleWall::BeginPlay()
{
	Super::BeginPlay();

	Target = GetWorld()->GetFirstPlayerController()->GetCharacter();
	
}
void ACParticleWall::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	NiagaraComp->SetVectorParameter("PlayerPosition", Target->GetActorLocation());
}

