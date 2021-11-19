#pragma once

#include "Client_Defines.h"
#include "Level.h"

BEGIN(Client)

class CLevel_Stage final : public CLevel
{
private:
	explicit CLevel_Stage(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual ~CLevel_Stage() = default;

public:
	virtual HRESULT	NativeConstruct() override;
	virtual _int	Tick(_double dTimedelta) override;
	virtual HRESULT	Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Terrain(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Cody(const _tchar* pLayerTag);
	HRESULT Ready_Layer_May(const _tchar* pLayerTag);
	HRESULT Ready_Layer_UFO(const _tchar* pLayerTag);
	HRESULT Ready_Layer_MoonBaboon(const _tchar* pLayerTag);

	/* Interactive Objects */
	HRESULT Ready_Layer_Rocket(const _tchar* pLayerTag);
	HRESULT Ready_Layer_StarBuddy(const _tchar* pLayerTag);
	HRESULT Ready_Layer_Robot(const _tchar* pLayerTag);
	HRESULT Ready_Layer_RobotHead(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SecurityCameraHandle(const _tchar* pLayerTag);
	HRESULT Ready_Layer_SecurityCamera(const _tchar* pLayerTag);

	/* For.Test */
	HRESULT Ready_Layer_Test();

public:
	static CLevel_Stage* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual void Free() override;
};

END