#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CStaticActor;
class CTriggerActor;
END

BEGIN(Client)
class CToyBoxButton_Button final : public CGameObject
{
private:
	explicit CToyBoxButton_Button(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CToyBoxButton_Button(const CToyBoxButton_Button& rhs);
	virtual ~CToyBoxButton_Button() = default;

public:
	void Set_ButtonOrigin();

public:
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

private:
	void OnPressed_Button(_double TimeDelta);

private:
	_int	m_iCheckTrigger = 0;
	_float	m_fTriggerDeltaMove = 0.f;

private:
	_float m_fPressedPercentage = 1.f;
	_float m_fPushButtonSoundVolume = 1.f;

private:
	/* For.Component */
	CRenderer* m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel* m_pModelCom = nullptr;
	CStaticActor* m_pStaticActorCom = nullptr;
	CTriggerActor* m_pTriggerActorCom = nullptr;

private:
	class CToyBoxButton* m_pParent = nullptr;

private:
	// 버튼 눌리면 일정량만큼 아래로 이동
	// 트리거 아웃이면, 다시 일정량만큼 위로 이동

public:
	static CToyBoxButton_Button* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg = nullptr);
	virtual CGameObject* Clone_GameObject(void* pArg) override { return this; };
	virtual void Free() override;
};
END
