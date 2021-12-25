#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
END

BEGIN(Client)
class CVolumeObject final : public CGameObject
{
public:
	enum TYPE { TYPE_SPHERE, TYPE_CUBE, TYPE_CYLINDER, TYPE_CONE, TYPE_END};

	typedef struct tagVolumeDesc
	{
		_float4x4	WorldMatrix				= MH_XMFloat4x4Identity();
		TYPE		eVolumeType				= TYPE_END;
		_float		fCullRadius				= 50.f;
		_float3		vInnerColor				= { 1.f,1.f,1.f };
		_float3		vOuterColor				= { 1.f,1.f,1.f };
	}VOLUME_DESC;

private:
	explicit CVolumeObject(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CVolumeObject(const CVolumeObject& rhs);
	virtual ~CVolumeObject() = default;

public:
	void Set_WorldMatrix(_fmatrix WorldMatrix);
	void Set_Color(_float3& vInner, _float3& vOuter);

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	VOLUME_DESC m_tVolumeDesc;
	_float m_fPatternDeltaT = 0.f;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel*				m_pModelCom = nullptr;
	CTextures*			m_pPatternTexCom = nullptr;

public:
	static CVolumeObject* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};
END