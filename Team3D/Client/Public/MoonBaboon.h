#pragma once

#include "Client_Defines.h"

BEGIN(Client)

class CMoonBaboon : public CGameObject
{
private:
	explicit CMoonBaboon(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CMoonBaboon(const CMoonBaboon& rhs);
	virtual ~CMoonBaboon() = default;

public:
	enum MOON_STATE {
		MOON_STATE_END
	};
	enum MOON_TARGET { TARGET_CODY, TARGET_MAY, TARGET_END };

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	CModel* Get_Model() const { return m_pModelCom; }

public:
	void Set_Animation(_uint iCurAnimIndex, _uint iNextAnimIndex);
	void Set_Active(_bool IsActive);

private:
	_bool				m_IsActive = true;

public:
	CTransform* Get_Transform() { return m_pTransformCom; }
	CModel*		Get_Model() { return m_pModelCom; }

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;

	/* For.UFO */ 
	CModel*				m_pUFOModel = nullptr;
	CTransform*			m_pUFOTransform = nullptr;

public:
	CMoonBaboon::MOON_STATE Check_State(_double dTimeDelta);
	void Change_State(_double dTimeDelta);
	void During_Animation_Behavior(_double dTimeDelta);
	void Fix_MoonBaboon_Chair(_double dTimeDelta);
	void SetUp_IntroOffset(_double dTimeDelta);
	void Set_CutSceneAnimation(_uint iAnimIdx, _double dTime);
private:
	MOON_STATE m_eCurState = MOON_STATE_END;
	MOON_STATE m_eNextState = MOON_STATE_END;
	MOON_TARGET m_eTarget = TARGET_END;
	_bool		m_bEjectOnce = false;
	_vector     m_vChairOffSetPos = {};
	_float		m_fEjectDelay = 0.f;

private:
	/* For.NativeConstruct */
	void Add_LerpInfo_To_Model();
	HRESULT Ready_Component();

public:
	static CMoonBaboon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END