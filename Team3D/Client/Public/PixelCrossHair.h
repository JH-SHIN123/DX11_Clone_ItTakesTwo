#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPixelCrossHair final : public CGameObject
 {
private:
	explicit CPixelCrossHair(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPixelCrossHair(const CGameObject& rhs);
	virtual ~CPixelCrossHair() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double dTimeDelta) override;
	virtual _int Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

 public: /* Getter */
	 CTransform* Get_Transform() { return m_pTransformCom; }

 private:
	 void Movement(_double dTimeDelta);

 private:
	 _float m_fInitialX = 0.f;
	 _float m_fInitialY = 0.f;

	 _float m_fMoveX = 0.f;
	 _float m_fMoveY = 0.f;

private:
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTextures*						m_pTextureCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;

public:
	static CPixelCrossHair* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END