#pragma once

#ifndef __EFFECT_CODY_SIZE_H__

#include "InGameEffect.h"

BEGIN(Client)
class CEffect_Cody_Size final : public CInGameEffect
{
private:
	enum SIZE_TYPE
	{
		TYPE_SMALL,
		TYPE_MIDDLE,
		TYPE_LARGE,
		TYPE_END
	};

private:
	explicit CEffect_Cody_Size(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CEffect_Cody_Size(const CEffect_Cody_Size& rhs);
	virtual ~CEffect_Cody_Size() = default;

public:
	virtual HRESULT	NativeConstruct_Prototype(void* pArg);
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double TimeDelta) override;
	virtual _int	Late_Tick(_double TimeDelta) override;
	virtual HRESULT	Render() override;

public:
	void Set_Model(CModel* pModel);				// Initialize
	void Update_Matrix(_fmatrix WorldMatrix);	// Tick
	void Change_SizeUp();
	void Change_SizeDown();

protected:
	virtual void Instance_Size(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_Pos(_float TimeDelta, _int iIndex = 0) override;
	virtual void Instance_UV(_float TimeDelta, _int iIndex = 0) override;

private:
	HRESULT Ready_Instance();
	void Check_Size(_double TimeDelta);	
	void Resizing_MS(_double TimeDelta);
	void Resizing_LM(_double TimeDelta);
	void Resizing_SM(_double TimeDelta);
	void Resizing_ML(_double TimeDelta);

	void Check_Resizing_End(_double TimeDelta);
	void Resizing_MS_End(_double TimeDelta);
	void Resizing_LM_End(_double TimeDelta);
	void Resizing_SM_End(_double TimeDelta);
	void Resizing_ML_End(_double TimeDelta);

private:
	_int Get_InstanceCount();
	

private:
	SIZE_TYPE m_eChangeSize_Cur		= TYPE_MIDDLE;
	SIZE_TYPE m_eChangeSize_Next	= TYPE_MIDDLE;

private:
	const _int	m_iInstance_Small	= 120;
	const _int	m_iInstance_Middle	= 600;
	const _int	m_iInstance_Large	= 1500;

	_double		m_dReSizeTime;
	_bool		m_IsReSizing = false;

private:
	CModel*		m_pTargetModel = nullptr;
	_float4*	m_pInstance_LocalPos = nullptr;
	_float*		m_pInstance_SizeType = nullptr;

public:
	static CEffect_Cody_Size* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, void* pArg);
	virtual CGameObject* Clone_GameObject(void* pArg) override;
	virtual void Free() override;

};

END
#define __EFFECT_CODY_SIZE_ML_H__
#endif
