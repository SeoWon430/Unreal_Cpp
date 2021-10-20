#include "CMontagesComponent.h"
#include "Global.h"
#include "GameFramework/Character.h"

UCMontagesComponent::UCMontagesComponent()
{
}


void UCMontagesComponent::BeginPlay()
{
	Super::BeginPlay();

	// GetRows from DataTable
	//	=> ����, �ǰ� ���� ������ �ൿ�� CActionData(DataAsset) �� �ƴ� DataTable�� ����
	TArray<FMontageData*> datas;
	DataTable->GetAllRows<FMontageData>("", datas);

	// �迭�� ����
	for (int32 i = 0; i < (int32)EStateType::Max; i++)
	{
		for (FMontageData* data : datas)
		{
			if ((EStateType)i == data->Type)
			{
				Datas[i] = data;
				continue;
			}
		}
	}

}

void UCMontagesComponent::PlayJump()
{
	PlayAnimMontage(EStateType::Jump);
}

void UCMontagesComponent::PlayTurn()
{
	PlayAnimMontage(EStateType::Turn);
}


void UCMontagesComponent::PlayHitted(float InPlayRate)
{
	// ��Ʈ��Ŀ������ �޾ƿ� �ǰݸ�� PlayRate�� ����
	PlayAnimMontage(EStateType::Hitted, InPlayRate);
}

void UCMontagesComponent::PlayDead()
{
	PlayAnimMontage(EStateType::Dead);
}

void UCMontagesComponent::PlayAnimMontage(EStateType InStateType, float InPlayRate)
{
	ACharacter* character = Cast<ACharacter>(GetOwner());

	FMontageData* data = Datas[(int32)InStateType];
	if (!!data)
	{
		if (!!data->AnimMontage)
			character->PlayAnimMontage(data->AnimMontage, data->PlayRatio * InPlayRate, data->StartSection);
	}

}


