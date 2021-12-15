#pragma once

#include "Client_Defines.h"
#include "UFORadarSet.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)

class CUFORadarLever final : public CUFORadarSet
{
	enum PREKEYINPUT { LEVER_LEFT, LEVER_RIGHT, LEVER_UP, LEVER_DOWN};

private:
	explicit CUFORadarLever(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CUFORadarLever(const CUFORadarSet& rhs);
	virtual ~CUFORadarLever() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;
	virtual HRESULT Render_ShadowDepth() override;
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject) override;

public: /* Getter */
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel* Get_Model() { return m_pModelCom; }

private:
	void Rotate_JoyStick(_double dTimeDelta);

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

private:
	RADARDESC	m_tRadarLeverDesc;
	_float      m_fAngleX = 0.f;
	_float      m_fAngleZ = 0.f;

	_float		m_fAngleXInverse = 0.f;
	_uint		m_iPreKeyInput = LEVER_LEFT;

public:
	static CUFORadarLever* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END