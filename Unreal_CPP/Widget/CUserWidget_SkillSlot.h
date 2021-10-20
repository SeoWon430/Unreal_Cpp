
#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_SkillSlot.generated.h"

UCLASS()
class U02_CPP_API UCUserWidget_SkillSlot : public UUserWidget
{
	GENERATED_BODY()


public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void UpdateCoolTime(float InRemainTime);
};
