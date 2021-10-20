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
    ProcMesh->bUseComplexAsSimpleCollision = false;	// 복합 콜리전 적용x
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
    /* StaticMesh는 가능하지만,
        SkeletalMesh는 재생중인 애니메이션에 따라 모양이 달라지기에 여기서 사용 불가
    */
    //UKismetProceduralMeshLibrary::CopyProceduralMeshFromStaticMeshComponent(StaticMesh, 0, ProcMesh, true);
}

void ACProceduralSkeletalMesh::BeginPlay()
{
    Super::BeginPlay();

    // 테스트로 게임 시작시 생성
    MakeMesh();
}


void ACProceduralSkeletalMesh::MakeMesh()
{
    CheckNull(SkeletalMesh->SkeletalMesh);
    CheckNull(DataTable);

    // 실제 SkeletalMesh를 ProceduralMesh로 생성
    CopySkeletalMeshToProcedural(SkeletalMesh, 0, ProcMesh);

    // DataTable에서 자를 소켓 정보를 가져옴
    //  => 사람 모양의 메시를 자를 부위(소켓) 위치를 특정 하기 위해 사용
    //  => 잘리고 난 부분을 어디로 날릴지 결정 (ex 왼손은 왼쪽으로 날림)
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
    // 잘라야 할 소켓 위치 리턴
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

    // 자르고 난 메시를 날릴 방향 체크
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
        ProcMesh,				// 잘리는 메시
        InHitResult.ImpactPoint,	// 잘리는 위치 (평면의 위치)
        sliceDirection,		// 잘리는 방향 (평면의 노말)
        true,					// 잘리고 독립된 메시들로 처리 할지
        outProcMesh,			// 잘리는 메시 리턴
        EProcMeshSliceCapOption::CreateNewSectionForCap,	// 잘리는 면에 Cap을 할지
        CapMaterial						// 잘린 면에 적용할 메터리얼
    );

    outProcMesh->SetConstraintMode(EDOFMode::Default);
    outProcMesh->SetSimulatePhysics(true);
    outProcMesh->SetEnableGravity(false);
    outProcMesh->bRenderCustomDepth = true;
    outProcMesh->CustomDepthStencilValue = 10.0f;   // CustomDepth 적용
    outProcMesh->MarkRenderStateDirty();            // 여기까지 해야 CustomDepth이 런타임에 적용 됨

    // 무게와 상관 없이 힘을 받음
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
    // 현재 SkeletalMesh에 대한 정보
    FSkeletalMeshRenderData* SkMeshRenderData = SkeletalMeshComponent->GetSkeletalMeshRenderData();

    // SkeletalMesh에 대한 LOD정보 => LOD에 따라 Vertex의 수가 달라지기 떄문
    const FSkeletalMeshLODRenderData& DataArray = SkMeshRenderData->LODRenderData[LODIndex];

    // 현재 SkeletalMesh를 만들때 설정 되었던 Skin Weight값 정보 => 애니메이션에 따라 Vertex의 위치가 바뀌기 때문
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

    // 메시의 Vertex 갯수를 가져옴
    int32 NumSourceVertices = DataArray.RenderSections[0].NumVertices;

    // 각 Vertex에 대하여 처리
    for (int32 i = 0; i < NumSourceVertices; i++)
    {
        //  Skin Weight를 고려한 Vertex의 위치
        FVector SkinnedVectorPos = USkeletalMeshComponent::GetSkinnedVertexPosition(SkeletalMeshComponent, i, DataArray, SkinWeights);
        VerticesArray.Add(SkinnedVectorPos);

        // 정점에 대한 방향 Normal과 Tangent를 구함
        FVector ZTangentStatic = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentZ(i);
        FVector XTangentStatic = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.VertexTangentX(i);

        Normals.Add(ZTangentStatic);
        Tangents.Add(FProcMeshTangent(XTangentStatic, false));

        // UV 처리
        FVector2D uvs = DataArray.StaticVertexBuffers.StaticMeshVertexBuffer.GetVertexUV(i, 0);
        UV.Add(uvs);

        // Vertex의 색상 처리 (사용 x)
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


    // ProceduralMesh 생성
    //ProcMeshComponent->CreateMeshSection(0, VerticesArray, Tris, Normals, UV, Colors, Tangents, true);
    ProcMeshComponent->CreateMeshSection_LinearColor(0, VerticesArray, Tris, Normals, UV, UVs1, UVs2, UVs3, Colors, Tangents, true);
    
    // 만들어진 의 콜리전 세팅
    ProcMeshComponent->ClearCollisionConvexMeshes();    // 초기화
    ProcMeshComponent->AddCollisionConvexMesh(VerticesArray);   // 정점들로부터 복합콜리전 생성
}