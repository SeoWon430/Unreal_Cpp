
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CWidgetDamageTextComponent.generated.h"

// 적에게 표시될 피격 데미지
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class U02_CPP_API UCWidgetDamageTextComponent : public USceneComponent
{
	GENERATED_BODY()

public:	
	UCWidgetDamageTextComponent();

protected:
	virtual void BeginPlay() override;

public:	
	void UpdateDamage(float InDamage, FName InText);

private:
	TArray<class ACDamageWidget*> Widgets;
	UINT Index;
};
