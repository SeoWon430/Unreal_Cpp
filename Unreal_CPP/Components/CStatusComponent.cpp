#include "CStatusComponent.h"
#include "Global.h"
#include "GameFramework/CharacterMovementComponent.h"


UCStatusComponent::UCStatusComponent()
{
	CurrentMove = ECharacterSpeed::Sprint;
	Health = OriginStatus.MaxHealth;
	Stamina = OriginStatus.MaxHealth;

	AddStatus.Atk = 0.0f;
	AddStatus.Def = 0.0f;
	AddStatus.HitRecovery = 0.0f;
	AddStatus.MotionSpeed = 0.0f;
	AddStatus.MaxHealth = 0.0f;
	AddStatus.MaxStamina = 0.0f;
}


void UCStatusComponent::BeginPlay()
{
	Super::BeginPlay();
}

// #################################################################
// Getter
// #################################################################
float UCStatusComponent::GetMaxHealth() 
{ 
	return (OriginStatus.MaxHealth + AddStatus.MaxHealth); 
}
float UCStatusComponent::GetHealthRatio() 
{ 
	return (Health / (OriginStatus.MaxHealth + AddStatus.MaxHealth)); 
}

float UCStatusComponent::GetMaxStamina() 
{ 
	return (OriginStatus.MaxStamina + AddStatus.MaxStamina); 
}
float UCStatusComponent::GetStaminaRatio() 
{ 
	return (Stamina / (OriginStatus.MaxStamina + AddStatus.MaxStamina)); 
}

float UCStatusComponent::GetAtk() 
{
	return (OriginStatus.Atk + AddStatus.Atk); 
}
float UCStatusComponent::GetDef() 
{ 
	return (OriginStatus.Def + AddStatus.Def); 
}
float UCStatusComponent::GetHitRecovery() 
{ 
	return (OriginStatus.HitRecovery + AddStatus.HitRecovery); 
}
float UCStatusComponent::GetMotionSpeed() 
{ 
	return (OriginStatus.MotionSpeed + AddStatus.MotionSpeed); 
}

float UCStatusComponent::GetWalkSpeed() 
{ 
	return MoveSpeed[(int32)ECharacterSpeed::Walk] * GetMotionSpeed(); 
}
float UCStatusComponent::GetRunSpeed() 
{ 
	return MoveSpeed[(int32)ECharacterSpeed::Run] * GetMotionSpeed(); 
}
float UCStatusComponent::GetSprintSpeed() 
{ 
	return MoveSpeed[(int32)ECharacterSpeed::Sprint] * GetMotionSpeed(); 
}




// #################################################################
// Setter
// #################################################################
void UCStatusComponent::AddHealth(float InAmount)
{
	Health += InAmount;

	Health = FMath::Clamp(Health, 0.0f, GetMaxHealth());
}
void UCStatusComponent::SubHealth(float InAmount)
{
	Health -= InAmount;
	Health = FMath::Clamp(Health, 0.0f, GetMaxHealth());
}


void UCStatusComponent::AddStamina(float InAmount)
{
	Stamina += InAmount;
	Stamina = FMath::Clamp(Stamina, 0.0f, GetMaxStamina());
}

void UCStatusComponent::SubStamina(float InAmount)
{
	Stamina -= InAmount;
	Stamina = FMath::Clamp(Stamina, 0.0f, GetMaxStamina());
}

void UCStatusComponent::AddAtk(float InAmount)
{
	AddStatus.Atk += InAmount;
}
void UCStatusComponent::AddDef(float InAmount)
{
	AddStatus.Def += InAmount;
}

void UCStatusComponent::AddHitRecovery(float InSpeed)
{
	AddStatus.HitRecovery += InSpeed;
}
void UCStatusComponent::AddMotionSpeed(float InSpeed)
{
	AddStatus.MotionSpeed += InSpeed;
	SetSpeed(CurrentMove);
}
void UCStatusComponent::SetHitMotion(bool InMotion)
{
	HitMotion = InMotion;
}

void UCStatusComponent::SetMove()
{
	bCanMove = true;
}

void UCStatusComponent::SetStop()
{
	bCanMove = false;
}


void UCStatusComponent::SetSpeed(ECharacterSpeed InSpeed)
{
	CurrentMove = InSpeed;
	UCharacterMovementComponent* movement = CHelpers::GetComponent<UCharacterMovementComponent>(GetOwner());
	movement->MaxWalkSpeed = MoveSpeed[(int32)InSpeed] * GetMotionSpeed();
}
