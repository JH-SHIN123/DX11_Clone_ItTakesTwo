#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CHpBar final : public CUIObject
 {
private:
	explicit CHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CHpBar(const CUIObject& rhs);
	virtual ~CHpBar() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	void Set_Active(_bool IsCheck);
	void Set_Hp(_float fHp);
	void Set_ShaderOption(_int iOption);

private:
	_bool							m_IsActive = false;
	_float							m_fHp = 0.f;
	_float							m_fRatio = 0.f;
	_float							m_fDecreaseRateRatio = 0.f;
	_float							m_fWatingTime = 0.f;
	_float							m_fRecoveryTime = 0.f;
	_float							m_fAngle = 0.f;
	_bool							m_IsHit = false;
	_bool							m_IsRecovery = false;
	_bool							m_IsChangeRotate = false;
	_uint							m_iOption = 0;
	_int							m_iShaderOption = 0;
	_float2							m_vMaxScale;
	_float2							m_vMinScale;

private:
	class CHpBarFrame*				m_pHpBarFrame = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_UI();

private:
	void Scale_Effect(_double TimeDelta);
	void Shake_Effect(_double TimeDelta);

private:
	 void CodyHpBar_Boss(_double TimeDelta);
	 void MayHpBar_Boss(_double TimeDelta);

public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END