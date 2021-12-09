#pragma once

#ifndef __SPACE_VALVE_STAR_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CSpace_Valve_Star final : public CGameObject
{
public:
	struct tagValve_Star_Desc
	{
		_float4x4	WorldMatrix;
		_bool		IsCody;
		tagValve_Star_Desc() {}
		tagValve_Star_Desc(_float4x4 World, _bool IsCodyValve)
			: WorldMatrix(World), IsCody(IsCodyValve) {}
	};
private:
	explicit CSpace_Valve_Star(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpace_Valve_Star(const CSpace_Valve_Star& rhs);
	virtual ~CSpace_Valve_Star() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render(RENDER_GROUP::Enum eGroup) override;

public:
	virtual HRESULT Render_ShadowDepth() override;

public:
	void Set_Clear_Level(_bool IsClear_Level);
	void Add_Rotate_Count(_int iCount);

private:
	HRESULT Ready_Component(void* pArg);
	HRESULT Ready_InstanceModel_Matrix();
	void	SetUp_WorldMatrix(_fmatrix WorldMatrix); // Only Once
	void	SetUp_Instance_World_Pos();
	void	Rotate_Check(_double TimeDelta);

private:
	void Rotate_Matrix(_bool IsReverseRotate, _float fAngle, _int iIndex);

private:
	_bool m_IsClear_Level = false;
	_int m_iRotate_Count = 0;
	_bool m_IsCodyValve = true;
	_bool m_IsValve_Max = false;
	_float m_fReset_Time = 0.f;

private: // 별 관련
	const _float3	m_vStar_Scale				= { 1.f, 1.f, 1.f };
	const _int		m_iModelInstance_Count		= 11;
	_float4x4*		m_pWorldMatrix_Star			= nullptr;
	_float			m_fRotateAngle[6]			= {};
	_float			m_fRotateAngle_Powwer[6]	= {};


private: // Door
	//enum EValveDoor {Right, Left, ValveDoor_End};
	//_float4x4 m_MatrixValve_Door[ValveDoor_End];
	const _float4 m_vOffsetPos_Door_R = { 0.5f,  -1.7f,  3.1f, 1.f };
	const _float4 m_vOffsetPos_Door_L = { 0.5f,  -1.7f, -3.1f, 1.f };
	_bool m_IsOpen = false;


private: // Base
	const _float4 m_vOffsetPos_Base = { 0.2f, -1.7f, 0.f, 1.f };

private:
	CRenderer*			m_pRendererCom				= nullptr;
	CTransform*			m_pTransformCom_Base		= nullptr; // 베이스 기준
	CModel*				m_pModelCom_Base			= nullptr;
	CStaticActor*		m_pStaticActCom_Bass		= nullptr;
	CModel_Instance*	m_pModelInstanceCom_Star	= nullptr; // 별 제너레이트 스타

private:
	class CSpace_Valve_Door* m_pSpace_Valve_Door_Right	= nullptr;
	class CSpace_Valve_Door* m_pSpace_Valve_Door_Left	= nullptr;

public:
	static CSpace_Valve_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __SPACE_VALVE_STAR_H__
#endif
