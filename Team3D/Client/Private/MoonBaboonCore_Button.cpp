#include "stdafx.h"
#include "MoonBaboonCore_Button.h"

CMoonBaboonCore_Button::CMoonBaboonCore_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CGameObject(pDevice, pDeviceContext)
{
}

CMoonBaboonCore_Button::CMoonBaboonCore_Button(const CMoonBaboonCore_Button& rhs)
	: CGameObject(rhs)
{
}

HRESULT CMoonBaboonCore_Button::NativeConstruct(void* pArg)
{
	CGameObject::NativeConstruct(pArg);

	return E_NOTIMPL;
}

_int CMoonBaboonCore_Button::Tick(_double TimeDelta)
{
	return _int();
}

_int CMoonBaboonCore_Button::Late_Tick(_double TimeDelta)
{
	return _int();
}

HRESULT CMoonBaboonCore_Button::Render(RENDER_GROUP::Enum eGroup)
{
	return E_NOTIMPL;
}

HRESULT CMoonBaboonCore_Button::Render_ShadowDepth()
{
	return E_NOTIMPL;
}

CMoonBaboonCore_Button* CMoonBaboonCore_Button::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg)
{
	return nullptr;
}

CGameObject* CMoonBaboonCore_Button::Clone_GameObject(void* pArg)
{
	return nullptr;
}

void CMoonBaboonCore_Button::Free()
{
}
