#pragma once

#ifndef __DATALOADER_H__

#include "Client_Defines.h"
#include "GameInstance.h"

BEGIN(Client)
class CDataLoader final
{
public:
	CDataLoader();
	~CDataLoader();

public:
	static HRESULT Load_EffectData(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);

};

END
#define __DATALOADER_H__
#endif // !__DATALOADER_H__
