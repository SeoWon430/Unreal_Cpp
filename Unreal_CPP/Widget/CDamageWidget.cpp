
#include "CDamageWidget.h"
#include "Global.h"
#include "Components/WidgetComponent.h"
#include "Widget/CUserWidget_DamageText.h"

ACDamageWidget::ACDamageWidget()
{
 	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateComponent(this, &Scene, "Scene");
	CHelpers::CreateComponent(this, &WidgetComp, "WidgetComp", Scene);

	TSubclassOf<UCUserWidget_DamageText> widgetClass;
	CHelpers::GetClass<UCUserWidget_DamageText>(&widgetClass, "WidgetBlueprint'/Game/Widget/WB_DamageText.WB_DamageText_C'");
	WidgetComp->SetWidgetClass(widgetClass);
	WidgetComp->SetRelativeRotation(FRotator(0, 90, 0));
	WidgetComp->SetDrawSize(FVector2D(120, 30));
	WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
}

void ACDamageWidget::BeginPlay()
{
	Super::BeginPlay();
	WidgetComp->SetVisibility(false);
	Widget = Cast<UCUserWidget_DamageText>(WidgetComp->GetUserWidgetObject());

}

void ACDamageWidget::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckTrue(Duration < 0.0f);
	Duration -= DeltaTime;

	FVector location = WidgetComp->GetRelativeLocation();
	location.Z += DeltaTime * 100.0f;
	WidgetComp->SetRelativeLocation(location);

	if (Duration < 0.0f)
		Reset();
}

void ACDamageWidget::UpdateDamage(float InDamage, FName InText)
{
	Duration = 1.0f;
	WidgetComp->SetVisibility(true);
	Widget->UpdateDamage(InDamage, InText);

	FVector location = FVector(0);
	location.Y = UKismetMathLibrary::RandomFloatInRange(-50, 50);
	location.Z = UKismetMathLibrary::RandomFloatInRange(50, 100);
	WidgetComp->SetRelativeLocation(location);
}


void ACDamageWidget:: Reset()
{
	WidgetComp->SetVisibility(false);
	WidgetComp->SetRelativeLocation(FVector(0));
	WidgetComp->SetRelativeScale3D(FVector(1));
}