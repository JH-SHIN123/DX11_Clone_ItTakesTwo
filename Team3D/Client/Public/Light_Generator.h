#pragma once

#include "Client_Defines.h"
#include "Base.h"

BEGIN(Client)
class CLight_Generator final : public CBase
{
	DECLARE_SINGLETON(CLight_Generator)

public:
	CLight_Generator() = default;
	virtual ~CLight_Generator() = default;
	
public:
	// Light에 이펙트 사용 여부 체크하기
	HRESULT	Add_Light(const _tchar* pLightTag, const LIGHT_DESC& LightDesc, _uint eEffectColor);
	// Clear_Light

	/* For. 조명설치 */
	// Move_Light
	// Delete_Light
	// Save_Light
	// Load_Light

public:
	virtual void Free() override;
};
END