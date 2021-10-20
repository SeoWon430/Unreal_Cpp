
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CDamageWidget.generated.h"

UCLASS()
class U02_CPP_API ACDamageWidget : public AActor
{
	GENERATED_BODY()
private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class UWidgetComponent* WidgetComp;

public:	
	ACDamageWidget();
	void UpdateDamage(float InDamage, FName InText);

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void Reset();

public:	
	virtual void Tick(float DeltaTime) override;

private:
	class UCUserWidget_DamageText* Widget;
	float Duration;
};
