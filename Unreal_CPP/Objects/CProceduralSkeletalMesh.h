#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "CProceduralSkeletalMesh.generated.h"

// �߶��� Mesh�� ���� ����
UENUM(BlueprintType)
enum class ESliceDirection : uint8
{
	Up, Down, Right, Left, Forward, Back
};

// �ڸ� ���� ����
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


// SkeletalMesh�κ��� ProceduralMesh ����
//	=> Skinning Weigth�� ����Ͽ� ���� ������� �ִϸ��̼Ǳ��� ���� ����
//	=> �ִϸ��̼��� ����� �� ������ SkeletalMesh�� ProceduralMesh�� ����
//	=> CActor_Cutter���� ���
UCLASS()
class U02_CPP_API ACProceduralSkeletalMesh : public AActor
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly, Category = "Slice")
		UDataTable* DataTable;

	UPROPERTY(EditDefaultsOnly, Category = "Slice")
		class UMaterialInstanceConstant* CapMaterial;	// �߶��� ���� ���͸���

	UPROPERTY(VisibleDefaultsOnly)
		class USceneComponent* Scene;

	UPROPERTY(VisibleDefaultsOnly)
		class USkeletalMeshComponent* SkeletalMesh;		// ProceduralMesh�� ���� �޽�

	UPROPERTY(VisibleDefaultsOnly)
		class UProceduralMeshComponent* ProcMesh;		// ProceduralMesh

	UPROPERTY(VisibleDefaultsOnly)
		class UCapsuleComponent* Collision;				// �׽�Ʈ�� �浹ü

public:	
	ACProceduralSkeletalMesh();

	virtual void OnConstruction(const FTransform& Transform) override;
	void CopySkeletalMeshToProcedural(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent);

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	void MakeMesh();		// SkeletalMesh�κ��� ProceduralMesh�� ����
	void Slice(FHitResult InHitResult, FVector UpVector);	// ������ ProceduralMesh�ڸ���
	FVector GetSliceLocation();

	UFUNCTION()
		void DestroyMesh();

private:
	TMap<class UProceduralMeshComponent*, FVector> OutProcs;
	TArray<FProceduralSliceSocket*> Datas;

	UINT Count;
};
