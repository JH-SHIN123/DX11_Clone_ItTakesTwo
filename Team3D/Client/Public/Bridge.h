#pragma once
#include "Client_Defines.h"
#include "GameObject.h"
#include "Instancing_Env.h"

BEGIN(Client)
class CBridge final : public CGameObject
{
private:
	explicit CBridge(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CBridge(const CBridge& rhs);
	virtual ~CBridge() = default;

public:
	void Call_Trigger();

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT NativeConstruct(void* pArg) override;
	virtual _int Tick(_double dTimeDelta) override;
	virtual _int Late_Tick(_double dTimeDelta) override;
	virtual HRESULT Render(RENDER_GROUP::Enum eGroup) override;

private:
	class CRenderer*			m_pRendererCom = nullptr;
	class CTransform*			m_pTransformCom = nullptr;
	class CModel_Instance*		m_pModelCom = nullptr;

	CInstancing_Env::ARG_DESC	m_Ins_Env_Desc;

	_float4x4					m_matPivot;
	_bool						m_bTrigger = false;
	_float						m_fAngle = 0;

public:
	static CBridge* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject * Clone_GameObject(void * pArg) override;
	virtual void Free() override;
};
END
