#ifndef Collider_h__
#define Collider_h__

#include "Component.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollider final : public CComponent
{
private:
	explicit CCollider(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CCollider(const CCollider& rhs);
	virtual ~CCollider() = default;

public:
	enum TYPE { TYPE_AABB, TYPE_OBB, TYPE_SPHERE, TYPE_END };

public:
	typedef struct tagColliderDesc
	{
		_float3		vScale;
		_float3		vRotation;
		_float3		vPosition;
	}COLLIDER_DESC;

public:
	virtual HRESULT	NativeConstruct_Prototype	(TYPE eColliderType);
	virtual HRESULT	NativeConstruct				(void* pArg) override;

public:
	virtual _int	Update_Collider	();
	virtual HRESULT	Render_Collider	();

private:
	TYPE									m_eColliderType	= TYPE_END;
	BoundingBox*							m_AABB;
	BoundingOrientedBox*					m_OBB;
	BoundingSphere*							m_Sphere;
	COLLIDER_DESC							m_ColliderDesc;
	_bool									m_isCollision	= false;
	_bool									m_isActivate	= false;
	/* For.Render */
	BasicEffect*							m_pEffect		= nullptr;
	ID3D11InputLayout*						m_pInputLayout	= nullptr;
	PrimitiveBatch<VertexPositionColor>*	m_pBatch		= nullptr;

public:
	static CCollider*	Create			(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, TYPE eColliderType);
	virtual CComponent* Clone_Component	(void* pArg) PURE;
	virtual void		Free			() override;
};

END

#endif // Collider_h__
