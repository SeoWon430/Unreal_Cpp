#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CProceduralSkeletalMesh.generated.h"

// 잘라진 Mesh를 날릴 방향
UENUM(BlueprintType)
enum class ESliceDirection : uint8
{
	Up, Down, Right, Left, Forward, Back
};

// 자를 소켓 정보
USTRUCT(BlueprintType)
struct FProceduralSliceSocket : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
		FName SocketName;

	UPROPERTY(EditAnywhere)
		ESliceDirection SliceDirection;

	UPROPERTY(EditAnywhere)
		float NextDelay = 0.1f;
	

	FVector SocketLocation;

};


// SkeletalMesh로부터 ProceduralMesh 생성
//	=> Skinning Weigth를 고려하여 현재 재생중인 애니메이션까지 적용 가능
//	=> 애니메이션이 적용된 한 순간의 SkeletalMesh를 ProceduralMesh로 생성
//	=> CActor_Cutter에서 사용
UCLASS()
class U02_CPP_API ACProceduralSkeletalMesh : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Slice")
		UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Slice")
		class UMaterialInstanceConstant* CapMaterial;	// 잘라진 면의 메터리얼

	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class USkeletalMeshComponent* SkeletalMesh;		// ProceduralMesh로 만들 메시

	UPROPERTY(VisibleDefaultsOnly)
		class UProceduralMeshComponent* ProcMesh;		// ProceduralMesh

	UPROPERTY(VisibleDefaultsOnly)
		class UCapsuleComponent* Collision;				// 테스트용 충돌체

public:	
	ACProceduralSkeletalMesh();

	virtual void OnConstruction(const FTransform& Transform) override;
	void CopySkeletalMeshToProcedural(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void MakeMesh();		// SkeletalMesh로부터 ProceduralMesh를 생성
	void Slice(FHitResult InHitResult, FVector UpVector);	// 생성된 ProceduralMesh자르기
	FVector GetSliceLocation();

	UFUNCTION()
		void DestroyMesh();

private:
	TMap<class UProceduralMeshComponent*, FVector> OutProcs;
	TArray<FProceduralSliceSocket*> Datas;

	UINT Count;
};
