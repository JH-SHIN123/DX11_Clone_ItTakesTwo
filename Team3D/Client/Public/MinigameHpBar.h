#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CMinigameHpBar final : public CUIObject
 {
public:
	 enum STAGE { Stage_Boss, Stage_Minigame, Stage_End };

private:
	explicit CMinigameHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMinigameHpBar(const CUIObject& rhs);
	virtual ~CMinigameHpBar() = default;

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
	void Set_ResetHp();

private:
	_bool							m_IsActive = false;
	_float							m_fHp = 0.f;
	_float							m_fMaxHp = 0.f;
	_float							m_fRatio = 0.f;
	_float							m_fDecreaseRateRatio = 0.f;
	_float							m_fWatingTime = 0.f;
	_float							m_fAngle = 0.f;
	_bool							m_IsHit = false;
	_bool							m_IsChangeRotate = false;
	_uint							m_iOption = 0;
	_int							m_iShaderOption = 0;
	_uint							m_iShaderPassNum = 0;

private:
	class CMinigameHpBarFrame*				m_pMinigameHpBarFrame = nullptr;
	class CPortrait*						m_pMinigamePortrait = nullptr;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;
	
private:
	HRESULT Ready_Component();
	HRESULT Ready_Layer_UI();

private:
	void Scale_Effect(_double TimeDelta);
	void Shake_Effect(_double TimeDelta);

private:
	 void CodyMinigameHpBar(_double TimeDelta);
	 void MayMinigameHpBar(_double TimeDelta);

public:
	static CMinigameHpBar* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END