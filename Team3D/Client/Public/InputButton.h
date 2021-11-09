#pragma once

#include "Ortho_UIObject.h"

BEGIN(Client)

class CInputButton final : public COrtho_UIObject
{
private:
	explicit CInputButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInputButton(const CInputButton& rhs);
	virtual ~CInputButton() = default;
};

END