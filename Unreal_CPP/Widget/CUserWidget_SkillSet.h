// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CUserWidget_SkillSet.generated.h"

/**
 * 
 */
UCLASS()
class U02_CPP_API UCUserWidget_SkillSet : public UUserWidget
{
	GENERATED_BODY()
		
public:
	UFUNCTION(BlueprintImplementableEvent)
		void UpdateCoolTimes(const TArray<float>& InTimes);
		
};
