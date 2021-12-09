#pragma once
#include "Client_Defines.h"

BEGIN(Client)
class CMoonBaboon_SpaceShip : public CGameObject
{
protected:
	explicit CMoonBaboon_SpaceShip(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboon_SpaceShip(const CMoonBaboon_SpaceShip& rhs);
	virtual ~CMoonBaboon_SpaceShip() = default;

public:
	CModel* Get_Model() { return m_pModelCom; };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

protected:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

public:
	static CMoonBaboon_SpaceShip* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END