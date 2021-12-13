#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPixelHeart;
class CPixelShield;

class CPixelBaboon final : public CGameObject
 {
private:
	explicit CPixelBaboon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPixelBaboon(const CGameObject& rhs);
	virtual ~CPixelBaboon() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double dTimeDelta) override;
	virtual _int Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	CVIBuffer_Rect*					m_pVIBufferCom = nullptr;
	CRenderer*						m_pRendererCom = nullptr;
	CTextures*						m_pTextureCom = nullptr;
	CTransform*						m_pTransformCom = nullptr;

 public: /* Setter */
	 void Set_IsHit(_bool _IsHit) { m_IsHit = _IsHit; }
	 void Set_Hearts_Pos();

 private:
	 void Check_3DMoonBaboon_Position(_double dTimeDelta);

 private:
	 _float m_fSizeX = 0.f;
	 _float m_fSizeY = 0.f;

	 _bool	m_IsHit = false;
	 _bool  m_bBlinking = false;
	 _bool	m_bBlink = false;

	 _float m_fBlinkingTime = 0.f;
	 _int	m_iBlinkingCount = 0;

	 _int	m_iLifeCount = 3;

	 CPixelHeart* m_pPixelHeart[3];
	 CPixelShield* m_pPixelShield = nullptr;


public:
	static CPixelBaboon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END