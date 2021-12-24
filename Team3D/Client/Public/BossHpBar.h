#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CBossHpBar final : public CUIObject
 {
private:
	explicit CBossHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBossHpBar(const CUIObject& rhs);
	virtual ~CBossHpBar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Ratio(_float fRatio);
	void Set_Active(_bool IsCheck);
	void Set_HpBarReduction(_float fDamage);

 private:
	_float							m_fRatio = 1.f;
	_float							m_fDecreaseRateRatio = 1.f;
	_float							m_fWaitingTime = 0.f;
	_bool							m_IsActive = false;
	_float							m_fScaleX = 0.f;
	_bool							m_IsFontRender = false;
	_float							m_fMaxHp = 0.f;
	_float							m_fHp = 0.f;

 private:
	 class CBossHpBarFrame*			m_pBossHpBarFrame = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;
	CFontDraw*						m_pFont = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_UI();

private:
	HRESULT Render_Font();
	void Scale_Effect(_double TimeDelta);

public:
	static CBossHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END