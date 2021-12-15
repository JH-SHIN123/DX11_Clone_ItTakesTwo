#pragma once

#include "Client_Defines.h"
#include "UFORadarSet.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CUFORadarScreen final : public CUFORadarSet
{
private:
	explicit CUFORadarScreen(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFORadarScreen(const CUFORadarSet& rhs);
	virtual ~CUFORadarScreen() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	HRESULT Set_MeshRenderGroup();
	HRESULT Add_GameObject_ToRenderGroup();

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	RADARDESC	m_tRadarScreenDesc;

public:
	static CUFORadarScreen* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END