#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CStatic_Env final : public CGameObject
{
private:
	explicit CStatic_Env(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStatic_Env(const CStatic_Env& rhs);
	virtual ~CStatic_Env() = default;

public:
	typedef struct tagArgumentDesc
	{
		_float4x4		WorldMatrix;
		_tchar			szModelTag[MAX_PATH];
		_uint			iMaterialIndex;
		_float			fCullRadius;
		GameID::Enum	eGameID;

	}ARG_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eRender) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

private:
	CTransform*			m_pTransformCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CStaticActor*		m_pStaticActorCom = nullptr;

	ARG_DESC			m_Static_Env_Desc;

private:
	HRESULT Ready_Component(void* pArg);
	HRESULT Set_Simulation();
	HRESULT Set_MeshRenderGroup();
	HRESULT Add_GameObject_ToRenderGroup();

public:
	static CStatic_Env* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END
