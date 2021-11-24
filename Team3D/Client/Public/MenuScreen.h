#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CMenuScreen final : public CUIObject
{
private:
	explicit CMenuScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMenuScreen(const CUIObject& rhs);
	virtual ~CMenuScreen() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_double							m_Time = 0.0;
	_uint							m_iScaleChangeCount = 0;
	_float2							m_vStartScale;
	_float							m_fPower = 0.f;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void Render_Font();
	HRESULT Ready_Component();

public:
	static CMenuScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END