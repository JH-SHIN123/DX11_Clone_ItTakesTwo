#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	explicit CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public: /* Getter */
	virtual _float	Get_DistanceFromCamera();
	const _bool		Is_Dead() const { return m_isDead; }

public: /* Setter */
	void Set_Dead();

public:
	virtual HRESULT	NativeConstruct_Prototype();
	virtual HRESULT	NativeConstruct(void* pArg);
	virtual _int	Tick(_double dTimeDelta);
	virtual _int	Late_Tick(_double dTimeDelta);
	virtual HRESULT	Render();
	/* For.Write Shadow Depth */
	virtual HRESULT Render_ShadowDepth();
	/* For.Trigger */
	virtual void	Trigger(TriggerStatus::Enum eStatus, GameID::Enum eID, CGameObject* pGameObject);

protected: /* Typedef */
	typedef unordered_map<const _tchar*, CComponent*> COMPONENTS;
protected:
	class CGameInstance*	m_pGameInstance = nullptr;
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;
	COMPONENTS				m_Components;
	_bool					m_isDead = false;
	_bool					m_isClone = false;
	USERDATA				m_UserData;
protected:
	MATERIAL_OBJECT			m_Material;
protected:
	HRESULT	Add_Component(_uint iPrototypeLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

private:
	CComponent*	Find_Component(const _tchar* pComponentTag);

public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() PURE;
};

END