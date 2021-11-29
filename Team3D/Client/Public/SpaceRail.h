#pragma once

#include "Client_Defines.h"

BEGIN(Client)
class CSpaceRail_Node;
class CSpaceRail : public CComponent
{
public:
	enum STATE { STATE_FORWARD, STATE_BACKWARD, STATE_END };

private:
	explicit CSpaceRail(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpaceRail(const CSpaceRail& rhs);
	virtual ~CSpaceRail() = default;

public:
	void	RideOnRail(const _tchar* pRailTag, _uint iIndex, STATE eState);
	void	Riding(class CCharacter* pSubject);

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);

private:
	class CSpaceRailData*			m_pRailData = nullptr;
	const vector<CSpaceRail_Node*>*	m_pSpaceRailNodes = nullptr;
	
private:
	_bool m_bRiding = false;
	STATE m_eState = STATE_END;
	_uint m_iIndex_CurNode = 0;

public:
	static CSpaceRail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone_Component(void* pArg) override;
	virtual void Free() override;
};
END
