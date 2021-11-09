#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CMainCamera final : public CCamera
{
private:
	explicit CMainCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMainCamera(const CMainCamera& rhs);
	virtual ~CMainCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Set_ShaderConstant_Default() override;
	virtual HRESULT Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix) override;

public:
	static CMainCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;
};

END