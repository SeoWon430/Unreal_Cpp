// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CCharacterCapture.generated.h"

// 캐릭터 캡쳐
//	=> Tab눌렀을 때 나오는 캐릭터 모습을 보여주기 위한 액터
UCLASS()
class U02_CPP_API ACCharacterCapture : public AActor
{
	GENERATED_BODY()
	
private:
	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class USkeletalMeshComponent* SkeletalMesh;

	UPROPERTY(VisibleDefaultsOnly)
		class USceneCaptureComponent2D* Capture;



public:	
	ACCharacterCapture();

	void StartCapture();
	void EndCapture();

protected:
	virtual void BeginPlay() override;


};
