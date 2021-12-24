#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CHpBar final : public CUIObject
 {
public:
	 enum STAGE { Stage_Boss, Stage_Minigame, Stage_End };

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
	void Set_Stage(CHpBar::STAGE eStage);

private:
	STAGE							m_eStage = Stage_End;
	_bool							m_IsActive = false;
	_float							m_fHp = 0.f;
	_float							m_fMaxHp = 0.f;
	_float							m_fRatio = 0.f;
	_float							m_fDecreaseRateRatio = 0.f;
	_float							m_fWatingTime = 0.f;
	_float							m_fRecoveryTime = 0.f;
	_float							m_fAngle = 0.f;
	_bool							m_IsHit = false;
	_bool							m_IsRecovery = false;
	_bool							m_IsChangeRotate = false;
	_bool							m_IsBossStage = false;
	_uint							m_iOption = 0;
	_int							m_iShaderOption = 0;
	_uint							m_iShaderPassNum = 0;

private:
	class CHpBarFrame*				m_pHpBarFrame = nullptr;
	class CPortrait*				m_pPortrait = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_UI();

private:
	void Shake_Effect(_double TimeDelta);

private:
	 void CodyHpBar_Boss(_double TimeDelta);
	 void MayHpBar_Boss(_double TimeDelta);
	 void CodyHpBar_Minigame(_double TimeDelta);
	 void MayHpBar_Minigame(_double TimeDelta);

public:
	static CHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END