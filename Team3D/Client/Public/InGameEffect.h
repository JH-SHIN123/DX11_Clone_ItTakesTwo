#pragma once

#ifndef __INGAMEEFFECT_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_PointInstance_Custom_ST;
class CTransform;
class CRenderer;
class CTextures;
class CModel;
END

BEGIN(Client)

typedef struct tagEffect_Desc_Prototype
{
	_tchar		EffectName[MAX_PATH] = L""; //PrototypeName
	_tchar		TextureName[MAX_PATH] = L"";
	_uint		iTextureNum = 0;
	_tchar		TextureName_Second[MAX_PATH] = L"";
	_uint		iTextureNum_Second = 0;
	_tchar		ModelName[MAX_PATH] = L"";

	_int		iInstanceCount = 1;			//인스턴싱 개수

	_int		iTextureCount_U = 1;			// 플립 이미지 일 경우 세팅
	_int		iTextureCount_V = 1;

	_float		fLifeTime = -1.f;			// 생명시간

	//_float		fSizeTime = 0.f;
	//_bool		IsResizeContinue = false; // 쏵 늘어났다 가 아니고 같은 값으로 증감
	//_float		fSizeChangePower = 0.f;
	_float3		vSize = { 1.f, 1.f, 1.f };
	//_float3		vSizeChange = { 0.f, 0.f, 0.f };
	//
	//_bool		IsGravity = false;		// 중력

	_int		iLevelIndex = 1;

	_bool		IsRePosAll = false;
	_float		fDirMoveSpeed = 1.f;

	_float		fRenderTerm = 0.f;				// 한번에 그리지 않고 하나씩 나타나듯이 그리는 주기
	_float		fInstancePosUpdateTerm = -1.f;	// 0번의 Pos로 다시 세팅되는 주기

	_float3		vPivotScale = { 0.01f, 0.01f, 0.01f };
	_float3		vPivotRotate_Degree = { 0.f, 0.f, 0.f };
}EFFECT_DESC_PROTO;

typedef struct tagEffect_Desc_Clone // 상황에 따라 얘는 좀 이랬으면 좋겠다 싶은 것들
{
	enum PLAYER_VALUE {	PV_MAY, PV_CODY, PV_CODY_S, PV_CODY_L, PV_END};

	_float3		vDir			= { 0.f, 0.f, 0.f };			// 0 Index Move Dir;
	_float3		vRandDirPower	= { 0.f, 0.f, 0.f };			// other Indeces Move Dir;
	_bool		IsRandDirDown[3] = { true, true, true };
	_bool		IsRandDir_FirstIndex = false;
	_float4x4	WorldMatrix = MH_XMFloat4x4Identity();
	_double		UVTime = -1.f;
	_bool		IsRandUV = false;
	_float		fSizePower = 1.f;
	_float3		vSize_Max = { 1.f,1.f,1.f };
	_int		iPlayerValue = PV_MAY;
	_float		fCullingRadius = 10.f;

	void* pArg = nullptr;
}EFFECT_DESC_CLONE; 

class CInGameEffect :	public CGameObject
{
protected:
	enum ERESOURCE_TYPE { RESOURCE_TEXTURE, RESOURCE_TEXTURE_SECOND, RESOURCE_MESH, RESOURCE_END };

protected:
	explicit CInGameEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CInGameEffect(const CInGameEffect& rhs);
	virtual ~CInGameEffect() = default; public:

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	HRESULT SetUp_InstanceCount(_uint iInstanceCount);
	HRESULT SetUp_Instance_WorldMatrix(_uint iIndex, _float4x4 WolrdMatrix);
	HRESULT SetUp_Instance_WorldMatrix(_uint iIndex, _fmatrix WolrdMatrix);
	HRESULT SetUp_Instance_Position(_uint iIndex, _fvector vPosition, _fvector vOffSetPosition = XMVectorZero());

public:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) PURE;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) PURE;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) PURE;


public: // 기본적인 기능
	void	Check_Color(_double TimeDelta);
	void	Check_Gravity(_double TimeDelta);
	_float4 Check_UV(_double TimeDelta, _int iIndex = 0, _bool IsLoop = true);
	_float2 Check_Size(_double TimeDelta, _int iIndex = 0);
	_float4 Check_Move(_double TimeDelta, _int iIndex = 0);

	void Control_InstanceBuffer(_double TimeDelta);

public:
	HRESULT Ready_Component(void* pArg);
	HRESULT	Ready_InstanceBuffer(_bool IsRenderTerm = false);

public:
	_float4 Set_UV(_int iIndex);
	void	SetUp_Shader_Data();
	_float4 Get_TexUV(_uint iTexture_U, _uint iTexture_V, _bool IsInitialize = false);
	_float4 Get_TexUV_Rand(_uint iTexture_U, _uint iTexture_V);
	_float3 Get_Dir_Rand(_int3 vRandDirPower);

protected:
	// 프로토타입에 기본 세팅을 하고 클론에서 상황에 맞는 추가적인 데이터를 넘기자
	EFFECT_DESC_PROTO m_EffectDesc_Prototype;			// 프로토 타입이 가져야 할 데이터
	EFFECT_DESC_CLONE m_EffectDesc_Clone;				// 클론 생성시 추가적인 세팅이 필요 할 때

protected:
	VTXMATRIX_CUSTOM_ST* m_pInstanceBuffer = nullptr;

	_int			m_iInstance_RenderCount = 1;

	_float3*		m_pInstance_Dir				= nullptr;
	_double*		m_pInstance_Pos_UpdateTime	= nullptr;

	_double*		m_pInstance_RenderTerm		= nullptr;
	_float2*		m_pInstance_UVCount			= nullptr;

	_double			m_UVTime = 0.0;

protected:
	CRenderer*	m_pRendererCom			= nullptr;
	CTransform*	m_pTransformCom			= nullptr;
	CTextures*	m_pTexturesCom			= nullptr;
	CTextures*	m_pTexturesCom_Second	= nullptr;
	CVIBuffer_PointInstance_Custom_ST* m_pPointInstanceCom = nullptr;

	CModel*		m_pModelCom = nullptr;

	_bool m_IsResourceName[RESOURCE_END];
	_bool m_IsBillBoard = true;

public:
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __INGAMEEFFECT_H__
#endif // !__INGAMEEFFECT_H__
