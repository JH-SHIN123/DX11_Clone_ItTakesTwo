#include "..\Public\PxContactCallback.h"

void CPxContactCallback::onContactModify(PxContactModifyPair * const pairs, PxU32 count)
{
	for (PxU32 iIndex = 0; iIndex < count; ++iIndex)
	{
		int i = 0;
		//pairs[iIndex].shape[0]
	}
}