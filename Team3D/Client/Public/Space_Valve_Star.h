#pragma once

#ifndef __SPACE_VALVE_STAR_H__

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Client)
class CSpace_Valve_Star final : public CGameObject
{
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
	void Set_WorldMatrix(_fmatrix WorldMatrix);

public:
	void Set_Clear_Level(_bool IsClear_Level);
	void Add_Rotate_Count(_int iCount);

private:
	HRESULT Ready_Component();
	HRESULT Ready_InstanceModel_Matrix();

private:
	void Open_ValveDoor();

private:
	_bool m_IsClear_Level = false;
	_int m_iRotate_Count = 0;

private: // 별 관련
	const _float3	m_vStar_Scale			= { 1.f, 1.f, 1.f };
	const _int		m_iModelInstance_Count	= 11;
	_float4x4*		m_pWorldMatrix_Star		= nullptr;

private: // Door
	enum EValveDoor {Right, Left, ValveDoor_End};
	_float4x4 m_MatrixValve_Door[ValveDoor_End];
	const _float3 m_vOffsetPos_Door_R = { 0.5f,  0.f,  3.1f };
	const _float3 m_vOffsetPos_Door_L = { 0.5f,  0.f, -3.1f };


private: // Base
	const _float3 m_vOffsetPos_Base = { 0.2f, -1.7f, 0.f };

private:
	CRenderer*			m_pRendererCom			= nullptr;
	CTransform*			m_pTransformCom			= nullptr; // 베이스 기준
	CModel*				m_pModelCom_Base		= nullptr;
	CModel*				m_pModelCom_Door_R		= nullptr;
	CModel*				m_pModelCom_Door_L		= nullptr;
	CModel_Instance*	m_pModelInstanceCom		= nullptr; // 별 제너레이트 스타


public:
	static CSpace_Valve_Star* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __SPACE_VALVE_STAR_H__
#endif
