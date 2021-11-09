#include "stdafx.h"
#include "..\Public\InputButton.h"


CInputButton::CInputButton(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: COrtho_UIObject(pDevice, pDeviceContext)
{
}

CInputButton::CInputButton(const CInputButton & rhs)
	: COrtho_UIObject(rhs)
{
}
