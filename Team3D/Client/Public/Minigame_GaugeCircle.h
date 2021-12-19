#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMinigame_GaugeCircle final : public CUIObject
 {
private:
	explicit CMinigame_GaugeCircle(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMinigame_GaugeCircle(const CUIObject& rhs);
	virtual ~CMinigame_GaugeCircle() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

public:
	 void Set_Active(_bool IsCheck);

public:
	 _bool Get_NextCount() const { return m_IsNextCount; }
	 _bool Get_ScaleDown() const { return m_IsScaleDown; }

private:
	_float							m_fAngle = 0.f;
	_float							m_fAlpha = 0.f;
	_bool							m_IsActive = false;
	_bool							m_IsScaleUp = false;
	_bool							m_IsNextCount = false;
	_bool							m_IsScaleDown = false;
	_float2							m_vMinScale;
	_float2							m_vSaveScale;
	_uint							m_iCount = 0;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();

public:
	static CMinigame_GaugeCircle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END