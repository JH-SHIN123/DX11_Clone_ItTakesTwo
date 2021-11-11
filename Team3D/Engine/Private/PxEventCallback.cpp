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
	for (PxU32 i = 0; i < nbPairs; i++)
	{
		const PxContactPair& cp = pairs[i];

		if (cp.events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
		{
			if ((!strcmp(pairHeader.actors[0]->getName(), "Player")) || (pairHeader.actors[1]->getName() == "Player"))
			{
				PxActor* pActor = ("Player" == pairHeader.actors[0]->getName()) ? pairHeader.actors[0] : pairHeader.actors[1];

				if ((pairHeader.actors[0]->getName() == "Ground") || (pairHeader.actors[1]->getName() == "Ground"))
				{
					int i = 0;
				}
				break;
			}
		}
	}
}

void CPxEventCallback::onTrigger(PxTriggerPair * pairs, PxU32 count)
{
	PX_UNUSED(pairs); PX_UNUSED(count);
}

void CPxEventCallback::onAdvance(const PxRigidBody * const * bodyBuffer, const PxTransform * poseBuffer, const PxU32 count)
{

}