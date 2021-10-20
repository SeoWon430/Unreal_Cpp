
#include "CWidgetDamageTextComponent.h"
#include "Global.h"
#include "Components/WidgetComponent.h"
#include "Widget/CDamageWidget.h"

UCWidgetDamageTextComponent::UCWidgetDamageTextComponent()
{
}


void UCWidgetDamageTextComponent::BeginPlay()
{
	Super::BeginPlay();

	ACDamageWidget* widget;

	for (int32 i = 0; i < 3; i++)
	{
		FTransform transform = FTransform();
		widget = GetWorld()->SpawnActorDeferred<ACDamageWidget>
			(
				ACDamageWidget::StaticClass(),
				transform,
				GetOwner(),
				nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		UGameplayStatics::FinishSpawningActor(widget, transform);
		widget->AttachToActor(GetOwner(), FAttachmentTransformRules(EAttachmentRule::KeepRelative, true));


		Widgets.Add(widget);
	}

}



void UCWidgetDamageTextComponent::UpdateDamage(float InDamage, FName InText)
{
	Widgets[Index++]->UpdateDamage(InDamage, InText);
	Index %= Widgets.Num();
}


