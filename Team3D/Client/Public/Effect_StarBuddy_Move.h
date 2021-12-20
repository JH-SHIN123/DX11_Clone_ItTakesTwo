#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CEffect_StarBuddy_Move final : public CGameObject
{
private:
	explicit CEffect_StarBuddy_Move(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_StarBuddy_Move(const CEffect_StarBuddy_Move& rhs);
	virtual ~CEffect_StarBuddy_Move() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;


public:
	static CEffect_StarBuddy_Move* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override; private:
};

END