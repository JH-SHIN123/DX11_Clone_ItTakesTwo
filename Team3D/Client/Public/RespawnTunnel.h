#pragma once

#ifndef __RESPAWNTUNNEL_H__

#include "Client_Defines.h"
#include "InGameEffect.h"

// BEGIN(Engine)
// class CVIBuffer_PointInstance_Custom;
// class CTransform;
// class CRenderer;
// class CTextures;
// class CModel;
// END

BEGIN(Client)
class CRespawnTunnel final : public CInGameEffect
{
private:
	explicit CRespawnTunnel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CRespawnTunnel(const CRespawnTunnel& rhs);
	virtual ~CRespawnTunnel() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	static CRespawnTunnel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __RESPAWNTUNNEL_H__
#endif // !__RESPAWNTUNNEL_H__
