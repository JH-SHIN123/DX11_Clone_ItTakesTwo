#pragma once

#ifndef __SPACE_VALVE_DOOR_H__

#include "GameObject.h"
BEGIN(Client)
class CSpace_Valve_Door final : public CGameObject
{
public:
	enum EDoor_Type { Right_Door, Left_Door, EDoor_Type_End };
	typedef struct tag_Arg_Desc
	{
		EDoor_Type	eDoorType;
		_bool		IsCodyDoor;
		tag_Arg_Desc() {}
		tag_Arg_Desc(EDoor_Type	eDoorType, _bool IsCodyDoor)
			: eDoorType(eDoorType), IsCodyDoor(IsCodyDoor) {}
	}ARG_DESC;

private:
	explicit CSpace_Valve_Door(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CSpace_Valve_Door(const CSpace_Valve_Door& rhs);
	virtual ~CSpace_Valve_Door() = default;

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
	void SetUp_WorldMatrix(_fmatrix WorldMatrix);

private:
	HRESULT Ready_Component(void* pArg);

private:
	void Open_ValveDoor();

private:
	_bool	m_IsCodyValve = true;
	_bool	m_IsClear_Level = false;

private: // Door
	const _float4 m_vOffsetPos_Door_R = { 0.5f,  -1.7f,  3.1f, 1.f };
	const _float4 m_vOffsetPos_Door_L = { 0.5f,  -1.7f, -3.1f, 1.f };
	_bool m_IsOpen = false;

private: // Base
	const _float4 m_vOffsetPos_Base = { 0.2f, -1.7f, 0.f, 1.f };

private:
	EDoor_Type			m_eDoorType			= EDoor_Type_End;
	CRenderer*			m_pRendererCom		= nullptr;
	CTransform*			m_pTransformCom		= nullptr;
	CModel*				m_pModelCom			= nullptr;
	CStaticActor*		m_pStaticActCom		= nullptr;

public:
	static CSpace_Valve_Door* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;
};

END
#define __SPACE_VALVE_DOOR_H__
#endif // !__SPACE_VALVE_DOOR_H__
