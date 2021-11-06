#ifndef Collision_Manager_h__
#define Collision_Manager_h__

#include "Base.h"
#include "DebugDraw.h"

BEGIN(Engine)

class ENGINE_DLL CCollision_Manager final : public CBase
{
	DECLARE_SINGLETON(CCollision_Manager)
private:
	explicit CCollision_Manager();
	virtual ~CCollision_Manager() = default;

public:
	HRESULT Ready_Collision_Manager		(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	HRESULT	Render_Colliders			();
	HRESULT	Add_Collider				(BoundingBox* pCollider);
	HRESULT	Add_Collider				(BoundingOrientedBox* pCollider);
	HRESULT	Add_Collider				(BoundingSphere* pCollider);
	void	Reset_Collider				(BoundingBox* pCollider);
	void	Reset_Collider				(BoundingOrientedBox* pCollider);
	void	Reset_Collider				(BoundingSphere* pCollider);
	/* For.ObjectRangeCollision */
	HRESULT	Add_ObjectRangeSphere		(BoundingSphere* pCollider);
	_bool	Is_Collided_ObjectRange		(_fvector vOldPos, _fvector vNewPos, _float& fOutDist, _vector& vOutRevised);
	/* For.AttackSphere */
	HRESULT Add_BodyOBBs				(class CGameObject* pDefender, vector<BoundingOrientedBox*>& Colliders);
	HRESULT	Delete_BodyOBBs				(class CGameObject* pDefender);
	HRESULT	Add_AttackSpheres			(class CGameObject* pAttacker, vector<BoundingSphere*>& Colliders);
	HRESULT Regenerate_AttackSpheres	(class CGameObject* pAttacker);
	HRESULT	Delete_AttackSpheres		(class CGameObject* pAttacker);
	HRESULT	Add_AttackOBBs				(class CGameObject* pAttacker, vector<BoundingOrientedBox*>& Colliders);
	HRESULT	Regenerate_AttackOBBs		(class CGameObject* pAttacker);
	HRESULT	Delete_AttackOBBs			(class CGameObject* pAttacker);
	_bool	Is_Collided_AttackSpheres	(class CGameObject* pAttacker, class CGameObject* pDefender);
	_bool	Is_Collided_AttackOBBs		(class CGameObject* pAttacker, class CGameObject* pDefender);
	/* For.BodyCollision */
	_bool	Is_Collided_Bodies			(class CGameObject* pCollider1, class CGameObject* pCollider2);
	/* For.Release */
	void	Clear_Colliders				();

private:
	typedef vector<BoundingBox*>							AABB_GROUP;
	typedef vector<BoundingOrientedBox*>					OBB_GROUP;
	typedef vector<BoundingSphere*>							SPHERE_GROUP;
	typedef pair<SPHERE_GROUP, vector<class CGameObject*>>	ATK_SPHERES;
	typedef pair<OBB_GROUP, vector<class CGameObject*>>		ATK_OBBS;
private:
	/* Render */
	AABB_GROUP											m_GroupAABB;
	OBB_GROUP											m_GroupOBB;
	SPHERE_GROUP										m_GroupSphere;
	/* For.Draw */
	ID3D11Device*										m_pDevice			= nullptr;
	ID3D11DeviceContext*								m_pDeviceContext	= nullptr;
	BasicEffect*										m_pEffect			= nullptr;
	ID3D11InputLayout*									m_pInputLayout		= nullptr;
	PrimitiveBatch<VertexPositionColor>*				m_pBatch			= nullptr;
	/* For.Collision_PushRange */
	SPHERE_GROUP										m_ObjectRangeSphere;
	/* For.Combat */
	unordered_map<class CGameObject*, OBB_GROUP>		m_BodyOBBs;
	unordered_map<class CGameObject*, ATK_SPHERES>		m_AttackSpheres;
	unordered_map<class CGameObject*, ATK_OBBS>			m_AttackOBBs;

public:
	virtual void	Free	() override;
};

END

#endif // Collision_Manager_h__