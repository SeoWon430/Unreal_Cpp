
#include "CWidgetPlayerComponent.h"
#include "Global.h"
#include "Components/WidgetComponent.h"
#include "Widget/CUserWidget_Menu.h"
#include "Widget/CUserWidget_PlayerStatus.h"
#include "Widget/CUserWidget_SkillSet.h"
#include "Character/CCharacterCapture.h"

UCWidgetPlayerComponent::UCWidgetPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	//if (!!GetOwner())
	//{
	//	// !!!!!!!!!!!!!!!!!!!!!!!!!!!
	//	UWidgetComponent* pComponent = GetOwner()->CreateDefaultSubobject<UWidgetComponent>("WidgetComp");
	//	pComponent->SetupAttachment(GetOwner()->GetRootComponent());
	//}


}


void UCWidgetPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	//WidgetMenuComp = NewObject<UWidgetComponent>(UWidgetComponent::StaticClass(), "WidgetMenuComp");
	//if (!!WidgetMenuComp)
	//{
	//	WidgetMenuComp->RegisterComponent();
	//	WidgetMenuComp->AttachTo(GetOwner()->GetRootComponent());
	//	WidgetMenuComp->SetWidgetClass(WidgetMenuClass);
	//}

	WidgetMenu = CreateWidget<UCUserWidget_Menu>(GetWorld(), WidgetMenuClass);
	WidgetStatus = CreateWidget<UCUserWidget_PlayerStatus>(GetWorld(), WidgetStatusClass);
	WidgetSkill = CreateWidget<UCUserWidget_SkillSet>(GetWorld(), WidgetSkillClass);

	WidgetMenu->AddToViewport();
	WidgetStatus->AddToViewport();
	WidgetSkill->AddToViewport();
	HideMenu();

	PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	UCStatusComponent* comp = Cast<UCStatusComponent>(GetOwner()->GetComponentByClass(UCStatusComponent::StaticClass()));
	if(!!comp)
		WidgetMenu->SetData(comp);

	
	if (!!CaptureClass)
	{

		FTransform transform;
		transform.SetLocation(FVector(10000, 0, 10000));
		RecordActor = GetWorld()->SpawnActorDeferred<ACCharacterCapture>
			(
				CaptureClass,
				transform,
				GetOwner(),
				nullptr,
				ESpawnActorCollisionHandlingMethod::AlwaysSpawn
				);
		UGameplayStatics::FinishSpawningActor(RecordActor, transform);
	}
	
}


void UCWidgetPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UCWidgetPlayerComponent::UpdateHealth(float InCurrent, float InMax)
{
	WidgetStatus->UpdateHealth(InCurrent, InMax);
}
void UCWidgetPlayerComponent::UpdateStamina(float InCurrent, float InMax)
{
	WidgetStatus->UpdateStamina(InCurrent, InMax);
}


void UCWidgetPlayerComponent::UpdateCoolTimes(const TArray<float>& InTime)
{
	WidgetSkill->UpdateCoolTimes(InTime);
}

void UCWidgetPlayerComponent::OnMenu()
{
	if (bMenuOpen == true)
		HideMenu();
	else
		ShowMenu();
}

void UCWidgetPlayerComponent::ShowMenu()
{
	bMenuOpen = true;

	WidgetMenu->bOpen = bMenuOpen;
	if(!!CaptureMaterial)
		WidgetMenu->SetCharacterImage(CaptureMaterial);
	WidgetMenu->SetVisibility(ESlateVisibility::Visible);
	WidgetStatus->SetVisibility(ESlateVisibility::Hidden);
	WidgetSkill->SetVisibility(ESlateVisibility::Hidden);

	if (!!PlayerController)
	{
		PlayerController->bShowMouseCursor = true;
		PlayerController->bEnableClickEvents = true;
		PlayerController->bEnableMouseOverEvents = true;
	}


	OriginTimeDilation = UGameplayStatics::GetGlobalTimeDilation(GetWorld());
	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.0f);

	if(!!RecordActor)
		RecordActor->StartCapture();
}
void UCWidgetPlayerComponent::HideMenu()
{
	bMenuOpen = false;
	WidgetMenu->bOpen = bMenuOpen;
	WidgetMenu->SetVisibility(ESlateVisibility::Hidden);
	WidgetStatus->SetVisibility(ESlateVisibility::Visible);
	WidgetSkill->SetVisibility(ESlateVisibility::Visible);


	if (!!PlayerController)
	{
		PlayerController->bShowMouseCursor = false;
		PlayerController->bEnableClickEvents = false;
		PlayerController->bEnableMouseOverEvents = false;
	}

	//UGameplayStatics::SetGlobalTimeDilation(GetWorld(), OriginTimeDilation);

	if (!!RecordActor)
		RecordActor->EndCapture();
}