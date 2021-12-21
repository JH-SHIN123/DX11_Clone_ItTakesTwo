#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Client)

class CInputButton_Frame final : public CUIObject
{
private:
	explicit CInputButton_Frame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInputButton_Frame(const CUIObject& rhs);
	virtual ~CInputButton_Frame() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual void Set_ScaleEffect() override;

private:
	_double							m_Time = 0.0;
	_uint							m_iScaleChangeCount = 0;
	_float2							m_vStartScale;
	_float2							m_vStartFontScale;
	_float2							m_vFontScale;
	_float							m_fPower = 0.f;
	_uint							m_iShaderPassNum = 0;
	_bool							m_IsScaleBigger = false;
	_bool							m_IsScaleSmaller = false;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void Render_Font();
	HRESULT Ready_Component();
	void SetUp_Option();
	void RespawnCircle_ScaleEffect();
	void ScaleEffect(_double TimeDelta);

public:
	static CInputButton_Frame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END