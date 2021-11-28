#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CDynamic_Env : public CGameObject
{
public:
	typedef struct tagArgumentDesc
	{
		_float4x4	WorldMatrix;
		_tchar		szModelTag[MAX_PATH];
		_uint		iMatrialIndex;
		_uint		iOption;
	}ARG_DESC;

protected:
	explicit CDynamic_Env(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CDynamic_Env(const CDynamic_Env& rhs);
	virtual ~CDynamic_Env() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

protected:
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;

	ARG_DESC		m_tDynamic_Env_Desc;

private:
	HRESULT Ready_Component(void* pArg);

public:
	virtual void Free() override;
};
END