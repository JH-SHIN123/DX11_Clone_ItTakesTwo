#include "stdafx.h"
#include "..\Public\PxEventCallback.h"

void CPxEventCallback::onConstraintBreak(PxConstraintInfo * constraints, PxU32 count)
{
	PX_UNUSED(constraints); PX_UNUSED(count);
}

void CPxEventCallback::onWake(PxActor ** actors, PxU32 count)
{
	PX_UNUSED(actors); PX_UNUSED(count);
}

void CPxEventCallback::onSleep(PxActor ** actors, PxU32 count)
{
	PX_UNUSED(actors); PX_UNUSED(count);
}

void CPxEventCallback::onContact(const PxContactPairHeader & pairHeader, const PxContactPair * pairs, PxU32 nbPairs)
{
	PX_UNUSED(pairHeader); PX_UNUSED(pairs); PX_UNUSED(nbPairs);

	//for (PxU32 i = 0; i < nbPairs; i++)
	//{
	//	const PxContactPair& cp = pairs[i];

	//	if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
	//	{
	//		//USERDATA* pTriggerUserData = static_cast<USERDATA*>(pairs[i].triggerActor->userData);
	//		//USERDATA* pActorUserData = static_cast<USERDATA*>(pairs[i].otherActor->userData);

	//		pTriggerUserData->pGameObject->Trigger(TriggerStatus::eFOUND, pActorUserData->eID, pActorUserData->pGameObject);

	//		if ((pairHeader.actors[0]->getName() == "Tower") || (pairHeader.actors[1]->getName() == "Tower"))
	//		{
	//			PxActor* pActor = ("Tower" == pairHeader.actors[0]->getName()) ? pairHeader.actors[0] : pairHeader.actors[1];

	//			if ((pairHeader.actors[0]->getName() == "Track") || (pairHeader.actors[1]->getName() == "Track"))
	//				((USERDATA*)(pActor->userData))->bDead = true;

	//			if ((pairHeader.actors[0]->getName() == "Player") || (pairHeader.actors[1]->getName() == "Player"))
	//				((USERDATA*)(pActor->userData))->bCollision = true;

	//			break;
	//		}
	//	}
	//}
}

void CPxEventCallback::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
	for (PxU32 i = 0; i < count; ++i)
	{
		if ((nullptr == pairs[i].triggerActor->userData) || (nullptr == pairs[i].otherActor->userData))
			continue;

		if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			USERDATA* pTriggerUserData	= static_cast<USERDATA*>(pairs[i].triggerActor->userData);
			USERDATA* pActorUserData	= static_cast<USERDATA*>(pairs[i].otherActor->userData);

			pTriggerUserData->pGameObject->Trigger(TriggerStatus::eFOUND, pActorUserData->eID, pActorUserData->pGameObject);
		}
		else
		{
			USERDATA* pTriggerUserData	= static_cast<USERDATA*>(pairs[i].triggerActor->userData);
			USERDATA* pActorUserData	= static_cast<USERDATA*>(pairs[i].otherActor->userData);

			pTriggerUserData->pGameObject->Trigger(TriggerStatus::eLOST, pActorUserData->eID, pActorUserData->pGameObject);
		}
	}
}

void CPxEventCallback::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{
	PX_UNUSED(bodyBuffer); PX_UNUSED(poseBuffer); PX_UNUSED(count);
}