#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel_Instance;
END

BEGIN(Client)
class CStaticVolume final : public CGameObject
{
public:
	typedef struct tagVolumeDesc
	{
		CModel_Instance::ARG_DESC	Instancing_Arg;
		_tchar						szModelTag[MAX_PATH] = L"";
		_float3*					arrInnerColor = nullptr; // 인스턴스 개수만큼
		_float3*					arrOuterColor = nullptr;
	}VOLUME_DESC;

private:
	explicit CStaticVolume(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CStaticVolume(const CStaticVolume& rhs);
	virtual ~CStaticVolume() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

private:
	VOLUME_DESC m_tVolumeDesc;
	_float4x4*	m_arrWorldMatrices_InnerColor = nullptr;
	_float4x4*	m_arrWorldMatrices_OuterColor = nullptr;

private:
	/* For.Component */
	CRenderer*			m_pRendererCom = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CModel_Instance*	m_pModelCom = nullptr;

public:
	static CStaticVolume* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};
END