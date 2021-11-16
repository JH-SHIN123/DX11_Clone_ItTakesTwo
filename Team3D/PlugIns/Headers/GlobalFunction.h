#pragma once

namespace Engine
{
	static void Setup_PxFiltering(PxRigidActor* pActor, PxU32 iFilterGroup, PxU32 iFilterMask)
	{
		PxFilterData FilterData;
		FilterData.word0 = iFilterGroup;
		FilterData.word1 = iFilterMask;

		const PxU32 iShapeCount = pActor->getNbShapes();
		PxShape** Shapes = new PxShape*[iShapeCount];
		pActor->getShapes(Shapes, iShapeCount);

		for (PxU32 iIndex = 0; iIndex < iShapeCount; ++iIndex)
		{
			PxShape* pShape = Shapes[iIndex];
			pShape->setSimulationFilterData(FilterData);
		}

		Safe_Delete_Array(Shapes);
	}
}