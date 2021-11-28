#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)

class CPressurePlate : public CGameObject
{
protected:
	explicit CPressurePlate(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CPressurePlate(const CPressurePlate& rhs);
	virtual ~CPressurePlate() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public:
	_bool Get_ButtonActive() { return m_IsButtonActive; }

public:
	void Set_Position(_vector vPosition);
	void Set_PipeCurveRotate(_bool IsCheck);

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }

private:
	_bool										m_IsButtonActive = false;
	_float										m_fMove = 0.f;
	_uint										m_iOption = 0;

protected:
	/* For.Component */
	CRenderer*									m_pRendererCom = nullptr;
	CTransform*									m_pTransformCom = nullptr;
	CModel*										m_pModelCom = nullptr;
	CStaticActor*								m_pStaticActorCom = nullptr;
	CTriggerActor*								m_pTriggerCom = nullptr;

private:
	class CPipeCurve*							m_pPipeCurve = nullptr;
	class CPressurePlateFrame*					m_pPressurePlateFrame = nullptr;
	class CSupportFrame*						m_pSupportFrame = nullptr;
	vector<class CPressurePlateLock*>			m_vecPressurePlateLock;

private:
	void SetUp_DefaultPositionSetting();
	void Button_Active(_double TimeDelta);

private:
	HRESULT Ready_Layer_PlateFrame(const _tchar * pLayerTag);
	HRESULT Ready_Layer_PipeCurve(const _tchar * pLayerTag);
	HRESULT Ready_Layer_SupportFrame(const _tchar * pLayerTag);
	HRESULT Ready_Layer_PlateLock(const _tchar * pLayerTag, _uint iCount);

public:
	static CPressurePlate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END