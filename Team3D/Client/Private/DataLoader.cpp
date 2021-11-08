#include "stdafx.h"
#include "..\Public\DataLoader.h"
#include "fstream"

#include "GameEffect.h"
#include "TestEffect.h"

CDataLoader::CDataLoader()
{
}


CDataLoader::~CDataLoader()
{
}

HRESULT CDataLoader::Load_EffectData(const _tchar * pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
	wifstream fin;

	fin.open(pFilePath);

	if (!fin.fail())
	{
		CGameInstance* pInstance = CGameInstance::GetInstance();
		Safe_AddRef(pInstance);

		while (true)
		{
			TCHAR NumData[MAX_PATH] = L"";

			EFFECT_DESC_PROTO* Data = new EFFECT_DESC_PROTO;

#pragma region Load Effect Data
			fin.getline(Data->EffectName, MAX_PATH, L'|');
			fin.getline(Data->TextureName, MAX_PATH, L'|');
			fin.getline(Data->TextureName_Second, MAX_PATH, L'|');
			fin.getline(Data->ModelName, MAX_PATH, L'|');

			if (fin.eof())
			{
				Safe_Delete(Data);
				break;
			}

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iInstanceCount = _ttoi(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iTextureCount_U = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');

			Data->iTextureCount_V = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fUVTime = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsFlowUV = (_ttoi(NumData) == TRUE) ? true : false;

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fLifeTime = _ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fSizeTime = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fSizeChangePower = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsResizeContinue = (_ttoi(NumData) == TRUE) ? true : false;
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSize.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vSizeChange.z = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fColorChangePower = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColor.w = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.z = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vColorChange.w = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsGravity = (_ttoi(NumData) == TRUE) ? true : false;

			fin.getline(NumData, MAX_PATH, L'|');
			Data->iShaderPass = _ttoi(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->iLevelIndex = _ttoi(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->IsRePosAll = (_ttoi(NumData) == TRUE) ? true : false;
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fDirMoveTime = _ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fDirMoveSpeed = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->fRenderTerm = _ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->fInstancePosUpdateTerm = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotScale.z = (_float)_ttof(NumData);

			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotRotate_Degree.x = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH, L'|');
			Data->vPivotRotate_Degree.y = (_float)_ttof(NumData);
			fin.getline(NumData, MAX_PATH);
			Data->vPivotRotate_Degree.z = (_float)_ttof(NumData);
#pragma endregion



			pInstance->Add_GameObject_Prototype(Data->iLevelIndex, Data->EffectName, CGameEffect::Create(pDevice, pDeviceContext, Data));
		}

		Safe_Release(pInstance);

		fin.close();
	}





	return S_OK;
}
