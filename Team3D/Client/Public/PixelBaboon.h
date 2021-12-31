#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPixelHeart;
class CPixelShield;
class CPixelArrow;

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

 public: /* Getter */
	 CTransform* Get_Transform() { return m_pTransformCom; }
	 _int		 Get_LifeCount() { return m_iLifeCount; }

 public: /* Setter */
	 void Set_IsHit(_bool _IsHit) { m_IsHit = _IsHit; }
	 void Set_Hearts_Pos();

 private:
	 void Check_Degree_And_Distance_From_MoonUFO(_double dTimeDelta);
	 void Check_Distance_From_UFO(_double dTimeDelta);

 private:
	 _float m_fSizeX = 0.f;
	 _float m_fSizeY = 0.f;

	 _bool	m_IsHit = false;
	 _bool  m_bBlinking = false;
	 _bool	m_bBlink = false;

	 _float m_fBlinkingTime = 0.f;
	 _int	m_iBlinkingCount = 0;

	 _int	m_iLifeCount = 3;
	 _bool  m_bRender = true;

	 _vector vCenterPos = {};

	 _bool  m_bTriggerOnce = false;

	 _bool	m_bStartHeartDelay = 0.f;
	 _float	m_fHeartDelay = 0.f;

	 CPixelHeart* m_pPixelHeart[3] = {nullptr};
	 CPixelShield* m_pPixelShield = nullptr;
	 CPixelArrow* m_pPixelArrow = nullptr;


 private:
	 CVIBuffer_Rect*				m_pVIBufferCom = nullptr;
	 CRenderer*						m_pRendererCom = nullptr;
	 CTextures*						m_pTextureCom = nullptr;
	 CTransform*					m_pTransformCom = nullptr;

public:
	static CPixelBaboon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END