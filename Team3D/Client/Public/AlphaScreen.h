#pragma once

#include "Client_Defines.h"
#include "UIObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CAlphaScreen final : public CUIObject
{
private:
	explicit CAlphaScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CAlphaScreen(const CUIObject& rhs);
	virtual ~CAlphaScreen() = default;

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct_Prototype(void* pArg);
	virtual HRESULT NativeConstruct(void * pArg) override;
	virtual _int Tick(_double TimeDelta) override;
	virtual _int Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	_float							m_fAlpha = 0.f;
	_float							m_fWatingTime = 0.f;
	_bool							m_IsFadeOut = false;
	_uint							m_iShaderPassNum = 0;

private:
	CVIBuffer_Rect*					m_pVIBuffer_RectCom = nullptr;

private:
	void Option_Setting();

private:
	HRESULT Ready_Component();

public:
	static CAlphaScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;

};

END