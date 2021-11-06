#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CCamera;
END

BEGIN(Client)
class CCharacter abstract : public CGameObject
{
protected:
	explicit CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	const _int Get_CurState() const { return m_iCurState; }
	const _int Get_NextState() const { return m_iNextState; }

	void Set_NextState(_int iState) { m_iNextState = iState; }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

protected:
	virtual void KeyProcess(_double TimeDelta) = 0;

protected: /* For.Component */
	CRenderer*	m_pRendererCom = nullptr;
	CTransform* m_pTransformCom = nullptr;
	CModel*		m_pModelCom = nullptr;

protected:
	_int m_iCurState = 0;
	_int m_iNextState = 0;

public:
	virtual void Free() override;
};
END