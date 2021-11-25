#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
protected:
	explicit CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public: /* Struct */
	typedef struct tagCameraDesc
	{
		_uint	iViewportIndex;

		/* For. Shadow */
		// Near, Fov는 안바뀐다고 가정함.
		_float	fFullScreenAspect;

		_float3	vEye;
		_float3	vAt;
		_float3	vAxisY;
		_float	fFovY;
		_float	fAspect;
		_float	fNear;
		_float	fFar;

		CTransform::TRANSFORM_DESC	TransformDesc;
	}CAMERA_DESC;

public: /* Getter */
	CCamera::CAMERA_DESC	Get_CameraDesc() { return m_CameraDesc; }
	virtual _fvector		Get_Position();

public: /* Setter */
	void Set_CamWorld(_fmatrix WorldMatrix) { m_pTransformCom->Set_WorldMatrix(WorldMatrix); }

public:
	virtual HRESULT NativeConstruct_Prototype() override;
	virtual HRESULT	NativeConstruct(void* pArg) override;
	virtual _int	Tick(_double dTimeDelta) override;
	virtual _int	Late_Tick(_double dTimeDelta) override;

protected:
	class CPipeline*	m_pPipeline = nullptr;
	CTransform*			m_pTransformCom = nullptr;
	CAMERA_DESC			m_CameraDesc;

public:
	virtual CGameObject* Clone_GameObject(void* pArg) PURE;
	virtual void Free() PURE;
};

END