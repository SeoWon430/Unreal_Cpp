
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CWidgetPlayerComponent.generated.h"


// 플레이어에게 사용될 스킬, HP,MP에 대한 UI
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U02_CPP_API UCWidgetPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	UPROPERTY(EditAnywhere)
		TSubclassOf<class ACCharacterCapture> CaptureClass;

	UPROPERTY(EditAnywhere)
		UMaterialInstance* CaptureMaterial;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCUserWidget_Menu> WidgetMenuClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCUserWidget_PlayerStatus> WidgetStatusClass;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class UCUserWidget_SkillSet> WidgetSkillClass;



public:	
	UCWidgetPlayerComponent();

	void OnMenu();

	void UpdateHealth(float InCurrent, float InMax);
	void UpdateStamina(float InCurrent, float InMax);

	void UpdateCoolTimes(const TArray<float>& InTime);

private:
	void ShowMenu();
	void HideMenu();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


private:
	class UCUserWidget_Menu* WidgetMenu;
	class UCUserWidget_PlayerStatus* WidgetStatus;
	class UCUserWidget_SkillSet* WidgetSkill;

	bool bMenuOpen;
	float OriginTimeDilation = 1.0f;
	
	APlayerController* PlayerController;

	class ACCharacterCapture* RecordActor;
};
