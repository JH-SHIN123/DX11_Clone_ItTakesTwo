#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPixelArrow final : public CGameObject
 {
private:
	explicit CPixelArrow(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPixelArrow(const CGameObject& rhs);
	virtual ~CPixelArrow() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double dTimeDelta) override;
	virtual _int Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

 public: /* Setter */
	 void Set_RenderState(_bool bRenderState) { m_bRender = bRenderState; }


 private:
	 _float m_fSizeX = 0.f;
	 _float m_fSizeY = 0.f;
	 _bool m_bRender = false;

 
 private:
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTextures*						m_pTextureCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;

public:
	static CPixelArrow* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END