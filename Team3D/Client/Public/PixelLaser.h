#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPixelLaser final : public CGameObject
 {
private:
	explicit CPixelLaser(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPixelLaser(const CGameObject& rhs);
	virtual ~CPixelLaser() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double dTimeDelta) override;
	virtual _int Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

 public:
	 void Set_Render_State(_bool bRender) { m_bRender = bRender; };

 private:
	 void Activate_Laser(_double dTimeDelta);


private:
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTextures*						m_pTextureCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;

 private:
	 _float m_fSizeX = 0.f;
	 _float m_fSizeY = 0.f;
	 _float	m_fAngle = 0.f;
	 _bool m_bRender = false;
	 _float m_fRenderTime = 0.f;



public:
	static CPixelLaser* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END