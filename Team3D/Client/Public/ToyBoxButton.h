#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
END

BEGIN(Client)
class CToyBoxButton final : public CGameObject
{
public:
	typedef struct tagToyBoxButtonDesc
	{
		_float4x4	WorldMatrix = MH_XMFloat4x4Identity();
		_uint		iIndex = 0;
	}TOYBOXBUTTON_DESC;

private:
	explicit CToyBoxButton(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToyBoxButton(const CToyBoxButton& rhs);
	virtual ~CToyBoxButton() = default;

public:
	_bool	Get_Trigger() { return m_bTrigger; }
	void	Set_Trigger() { m_bTrigger = true; }

public:
	CTransform* Get_Transform() const;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	void OnTrigger(_double TimeDelta);

private:
	TOYBOXBUTTON_DESC m_tDesc;

private:
	_bool	m_bTrigger = false;

	_uint	m_bCheckTrigger = 0;
	_float	m_fTriggerRatio = 0.f;
	_float	m_fToyBoxButtonRecoverVolume = 1.f;

private:
	/* For.Component */
	CRenderer*		m_pRendererCom = nullptr;
	CTransform*		m_pTransformCom = nullptr;
	CModel*			m_pModelCom = nullptr;
	CStaticActor*	m_pStaticActorCom = nullptr;

private:
	class CToyBoxButton_Button* m_pToyBoxButton_Button = nullptr;

private:
	// 트리거 받아서 메이의 내려찍기 ok
	// 메이의 중력방향으로 박스버튼 이동 (아니면 기존 look 방향으로 앞으로 나가도될듯)
	// 일정속도로 나아가고, 박스가 천천히 들어감.

public:
	static CToyBoxButton* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};
END