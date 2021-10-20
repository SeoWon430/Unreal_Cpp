#include "CProceduralSkeletalMesh.h"
#include "Global.h"
#include "KismetProceduralMeshLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Rendering/SkeletalMeshRenderData.h"
#include "Components/CapsuleComponent.h"

ACProceduralSkeletalMesh::ACProceduralSkeletalMesh()
{
    PrimaryActorTick.bCanEverTick = true;

    CHelpers::CreateComponent(this, &Scene, "Scene");
    CHelpers::CreateComponent(this, &Collision, "CollisionComp", Scene);
    CHelpers::CreateComponent(this, &SkeletalMesh, "SkeletalMesh", Collision);
    CHelpers::CreateComponent(this, &ProcMesh, "ProcMesh", Scene);


    USkeletalMesh* mesh;
    CHelpers::GetAsset<USkeletalMesh>(&mesh, "SkeletalMesh'/Game/Character/Mesh/SK_Mannequin.SK_Mannequin'");
    SkeletalMesh->SetSkeletalMesh(mesh);
    SkeletalMesh->SetRelativeLocation(FVector(0, 0, -88));
    SkeletalMesh->SetRelativeRotation(FRotator(0, -90, 0));
    TSubclassOf<UAnimInstance> animClass;
    CHelpers::GetClass<UAnimInstance>(&animClass, "AnimBlueprint'/Game/Player/ABP_CPlayer.ABP_CPlayer_C'");
    SkeletalMesh->SetAnimInstanceClass(animClass);
    SkeletalMesh->SetVisibility(false);
    SkeletalMesh->SetCollisionProfileName("NoCollision");


    ProcMesh->SetSimulatePhysics(true);
    ProcMesh->bUseComplexAsSimpleCollision = false;	// ���� �ݸ��� ����x
    ProcMesh->SetRelativeRotation(FRotator(0, -90, 0));
    ProcMesh->bRenderCustomDepth = true;
    ProcMesh->CustomDepthStencilValue = 10.0f;

    ProcMesh->SetConstraintMode(EDOFMode::Default);
    ProcMesh->GetBodyInstance()->bLockRotation = false;
    ProcMesh->GetBodyInstance()->bLockTranslation = false;

    Collision->SetCapsuleRadius(34.0f);
    Collision->SetCapsuleHalfHeight(88.0f);
    Collision->SetRelativeLocation(FVector(0, 0, 88));
    Collision->SetHiddenInGame(false);
}

void ACProceduralSkeletalMesh::OnConstruction(const FTransform& Transform)
{
    /* StaticMesh�� ����������,
        SkeletalMesh�� ������� �ִϸ��̼ǿ� ���� ����� �޶����⿡ ���⼭ ��� �Ұ�
    */
    //UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMesh, 0, ProcMesh, true);
}

void ACProceduralSkeletalMesh::BeginPlay()
{
    Super::BeginPlay();

    // �׽�Ʈ�� ���� ���۽� ����
    MakeMesh();
}


void ACProceduralSkeletalMesh::MakeMesh()
{
    CheckNull(SkeletalMesh->SkeletalMesh);
    CheckNull(DataTable);

    // ���� SkeletalMesh�� ProceduralMesh�� ����
    CopySkeletalMeshToProcedural(SkeletalMesh, 0, ProcMesh);

    // DataTable���� �ڸ� ���� ������ ������
    //  => ��� ����� �޽ø� �ڸ� ����(����) ��ġ�� Ư�� �ϱ� ���� ���
    //  => �߸��� �� �κ��� ���� ������ ���� (ex �޼��� �������� ����)
    DataTable->GetAllRows<FProceduralSliceSocket>("", Datas);
    for (FProceduralSliceSocket* data : Datas)
    {
        data->SocketLocation = SkeletalMesh->GetSocketLocation(data->SocketName);
    }
    Count = 0;
}

void ACProceduralSkeletalMesh::DestroyMesh()
{
    this->GetWorld()->DestroyActor(this);
}

void ACProceduralSkeletalMesh::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}

FVector ACProceduralSkeletalMesh::GetSliceLocation()
{
    // �߶�� �� ���� ��ġ ����
    if (Count < (UINT)Datas.Num())
        return Datas[Count]->SocketLocation;

    return FVector(0);
}


void ACProceduralSkeletalMesh::Slice(FHitResult InHitResult, FVector UpVector)
{
    CheckNull(DataTable);
    CheckNull(ProcMesh);
    CheckTrue(Count >= (UINT)Datas.Num());
    //UProceduralMeshComponent* procMesh = Cast<UProceduralMeshComponent>(InHitResult.Component);

    // �ڸ��� �� �޽ø� ���� ���� üũ
    FVector sliceDirection;
    switch (Datas[Count]->SliceDirection)
    {
    case ESliceDirection::Up:       sliceDirection = -GetActorUpVector();    break;
    case ESliceDirection::Down:     sliceDirection = GetActorUpVector();   break;
    case ESliceDirection::Right:    sliceDirection = GetActorRightVector(); break;
    case ESliceDirection::Left:     sliceDirection = -GetActorRightVector();    break;
    case ESliceDirection::Forward:  sliceDirection = GetActorForwardVector();   break;
    case ESliceDirection::Back:     sliceDirection = -GetActorForwardVector();  break;
    default:    break;
    }


    UProceduralMeshComponent* outProcMesh = nullptr;
    UKismetProceduralMeshLibrary::SliceProceduralMesh
    (
        ProcMesh,				// �߸��� �޽�
        InHitResult.ImpactPoint,	// �߸��� ��ġ (����� ��ġ)
        sliceDirection,		// �߸��� ���� (����� �븻)
        true,					// �߸��� ������ �޽õ�� ó�� ����
        outProcMesh,			// �߸��� �޽� ����
        EProcMeshSliceCapOption::CreateNewSectionForCap,	// �߸��� �鿡 Cap�� ����
        CapMaterial						// �߸� �鿡 ������ ���͸���
    );

    outProcMesh->SetConstraintMode(EDOFMode::Default);
    outProcMesh->SetSimulatePhysics(true);
    outProcMesh->SetEnableGravity(false);
    outProcMesh->bRenderCustomDepth = true;
    outProcMesh->CustomDepthStencilValue = 10.0f;   // CustomDepth ����
    outProcMesh->MarkRenderStateDirty();            // ������� �ؾ� CustomDepth�� ��Ÿ�ӿ� ���� ��

    // ���Կ� ��� ���� ���� ����
    outProcMesh->AddImpulse(sliceDirection * 300.0f, NAME_None, true);
    FVector location = outProcMesh->GetComponentLocation();
    FVector direction = FMath::VRand();
    OutProcs.Add(outProcMesh, direction);
    
    Count++;
    if (Count == Datas.Num())
    {
        UKismetSystemLibrary::K2_SetTimer(this, "DestroyMesh", 10.0f, false);
        //CLog::Print("end");
    }

}




void ACProceduralSkeletalMesh::CopySkeletalMeshToProcedural(USkeletalMeshComponent* SkeletalMeshComponent, int32 LODIndex, UProceduralMeshComponent* ProcMeshComponent)
{
    // ���� SkeletalMesh�� ���� ����
    FSkeletalMeshRenderData* SkMeshRenderData = SkeletalMeshComponent->GetSkeletalMeshRenderData();

    // SkeletalMesh�� ���� LOD���� => LOD�� ���� Vertex�� ���� �޶����� ����
    const FSkeletalMeshLODRenderData& DataArray = SkMeshRenderData->LODRenderData[LODIndex];

    // ���� SkeletalMesh�� ���鶧 ���� �Ǿ��� Skin Weight�� ���� => �ִϸ��̼ǿ� ���� Vertex�� ��ġ�� �ٲ�� ����
    FSkinWeightVertexBuffer& SkinWeights = *SkeletalMeshComponent->GetSkinWeightBuffer(LODIndex);

    TArray<FVector> VerticesArray;
    TArray<FVector> Normals;
    TArray<FVector2D> UV;
    TArray<FVector2D> UVs1;
    TArray<FVector2D> UVs2;
    TArray<FVector2D> UVs3;
    TArray<int32> Tris;
    TArray<FLinearColor> Colors;
    TArray<FProcMeshTangent> Tangents;

    // �޽��� Vertex ������ ������
    int32 NumSourceVertices = DataArray.RenderSections[0].NumVertices;

    // �� Vertex�� ���Ͽ� ó��
    for (int32 i = 0; i < NumSourceVertices; i++)
    {
        //  Skin Weight�� ����� Vertex�� ��ġ
        FVector SkinnedVectorPos = USkeletalMeshComponent::GetSkinnedVertexPosition(SkeletalMeshComponent, i, DataArray, SkinWeights);
        VerticesArray.Add(SkinnedVectorPos);

        // ������ ���� ���� Normal�� Tangent�� ����
        FVector ZTangentStatic = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
        FVector XTangentStatic = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(i);

        Normals.Add(ZTangentStatic);
        Tangents.Add(FProcMeshTangent(XTangentStatic, false));

        // UV ó��
        FVector2D uvs = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0);
        UV.Add(uvs);

        // Vertex�� ���� ó�� (��� x)
        Colors.Add(FLinearColor(0, 0, 0, 255));
    }


    // IndexBuffer
    FMultiSizeIndexContainerData indicesData;
    DataArray.MultiSizeIndexContainer.GetIndexBuffer(indicesData.Indices);

    //iterate over num indices and add traingles
    for (int32 i = 0; i < indicesData.Indices.Num(); i++)
    {
        uint32 a = 0;
        a = indicesData.Indices[i];
        Tris.Add(a);
    }


    // ProceduralMesh ����
    //ProcMeshComponent->CreateMeshSection(0, VerticesArray, Tris, Normals, UV, Colors, Tangents, true);
    ProcMeshComponent->CreateMeshSection_LinearColor(0, VerticesArray, Tris, Normals, UV, UVs1, UVs2, UVs3, Colors, Tangents, true);
    
    // ������� �� �ݸ��� ����
    ProcMeshComponent->ClearCollisionConvexMeshes();    // �ʱ�ȭ
    ProcMeshComponent->AddCollisionConvexMesh(VerticesArray);   // ������κ��� �����ݸ��� ����
}