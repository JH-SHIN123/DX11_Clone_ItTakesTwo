#pragma once
#include "GameObject.h"

BEGIN(Client)
class C3DText final : public CGameObject
{
public:
	typedef struct tagArgument
	{
		_float3		   vPosition = {};
		const _tchar*  szModelTag = nullptr;
		_bool		   IsBoost = false;
	}ARG_DESC;

private:
	explicit C3DText(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit C3DText(const C3DText& rhs);
	virtual ~C3DText() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTriggerActor*		m_pTriggerActorCom = nullptr;

	_bool				m_IsBoost = false;
	_float				m_fScale = 1.f;

private:
	HRESULT Ready_Component(void* pArg);

public:
	static C3DText* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END