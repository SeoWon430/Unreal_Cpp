
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_PlayerStatus.generated.h"


UCLASS()
class U02_CPP_API UCUserWidget_PlayerStatus : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateHealth(float InHealth, float InMaxHealth);

	UFUNCTION(BlueprintImplementableEvent)
		void UpdateStamina(float InStamina, float InMaxStamina);
};
