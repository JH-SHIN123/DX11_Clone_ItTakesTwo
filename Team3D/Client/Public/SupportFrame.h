#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CSupportFrame : public CGameObject
{
protected:
	explicit CSupportFrame(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSupportFrame(const CSupportFrame& rhs);
	virtual ~CSupportFrame() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	void Set_Position(_vector vPosition);
	void Set_LockActive(_bool IsCheck);

public:
	virtual HRESULT Render_ShadowDepth() override;


public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool										m_IsStart = true;
	_uint										m_iOption = 0;

private:
	vector<class CPressurePlateLock*>			m_vecPressurePlateLock;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;

private:
	void SetUp_DefaultPositionSetting();

private:
	HRESULT Ready_Layer_PlateLock(const _tchar * pLayerTag, _uint iCount);

public:
	static CSupportFrame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END