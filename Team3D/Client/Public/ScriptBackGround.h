#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CScriptBackGound : public CGameObject
{
protected:
	explicit CScriptBackGound(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CScriptBackGound(const CScriptBackGound& rhs);
	virtual ~CScriptBackGound() = default;

public:
	void Set_Position(_vector vPosition) { m_pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition); }

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup, _float2 fScale);

private:
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;

	CTextures*			m_pTextureCom = nullptr;
	CVIBuffer_Rect*		m_pVIBuffer_RectCom = nullptr;

private:
	HRESULT Ready_Component();
	HRESULT Set_UIVariables_Perspective();

public:
	static CScriptBackGound* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END