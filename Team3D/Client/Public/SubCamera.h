#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CSubCamera final : public CCamera
{
private:
	explicit CSubCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSubCamera(const CSubCamera& rhs);
	virtual ~CSubCamera() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Set_ShaderConstant_Default() override;
	virtual HRESULT Set_ShaderConstant_Shadow(_fmatrix LightViewMatrix, _fmatrix LightProjMatrix) override;

public:
	static CSubCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg = nullptr) override;
	virtual void Free() override;
};

END