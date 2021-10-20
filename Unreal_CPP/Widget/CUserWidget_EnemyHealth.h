
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_EnemyHealth.generated.h"


UCLASS()
class U02_CPP_API UCUserWidget_EnemyHealth : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void Update(float InHealth, float InMaxHealth);
	
};
