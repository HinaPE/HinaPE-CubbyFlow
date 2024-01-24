#include "GAS_VDB_TEST.h"

#include <SIM/SIM_DopDescription.h>
#include <PRM/PRM_Name.h>
#include <PRM/PRM_Template.h>

#include <SIM/SIM_FieldSampler.h>
#include <SIM/SIM_ScalarField.h>
#include <SIM/SIM_VectorField.h>
#include <SIM/SIM_MatrixField.h>

#include <GEO/GEO_PrimVDB.h>
#include <GU/GU_PrimVDB.h>

#include <GAS/GAS_Viscosity.h>

#include <openvdb/openvdb.h>

bool GAS_VDB_TEST::solveGasSubclass(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep)
{
	openvdb::GridBase::Ptr vdbPtrBase = nullptr;

	SIM_DataArray src, dst;
	getMatchingData(src, obj, GAS_NAME_FIELDSOURCE);
	getMatchingData(dst, obj, GAS_NAME_FIELDDEST);
	return true;
}

void GAS_VDB_TEST::addFieldsPartial(SIM_RawField *dst, const SIM_RawField *src, const UT_JobInfo &info)
{
	UT_VoxelArrayIteratorF vit;
	UT_Interrupt *boss = UTgetInterrupt();

	// Initialize our iterator to run over our destination field.
	vit.setArray(dst->fieldNC());

	// When we complete each tile the tile is tested to see if it can be
	// compressed, ie, is now constant.  If so, it is compressed.
	vit.setCompressOnExit(true);

	// Restrict our iterator only over part of the range.  Using the
	// info parameters means each thread gets its own subregion.
	vit.setPartialRange(info.job(), info.numJobs());

	// Create a sampler for the source field.
	SIM_ScalarFieldSampler srcsampler(dst, src);
	float srcval;

	// Visit every voxel of the destination array.
	for (vit.rewind(); !vit.atEnd(); vit.advance())
	{
		if (vit.isStartOfTile())
		{
			if (boss->opInterrupt())
				break;

			// Check if both source and destination tiles are constant.
			if (vit.isTileConstant() &&
				srcsampler.isTileConstant(vit, srcval))
			{
				// If both are constant, we can process the whole tile at
				// once. We call skipToEndOfTile() here so that the loop's
				// call to advance() will move us to the next tile.
				vit.getTile()->makeConstant( vit.getValue() + srcval );
				vit.skipToEndOfTile();
				continue;
			}
		}

		// Write out the sum of the two fields. Instead of using the
		// iterator, we could also have built a UT_VoxelRWProbeF.
		float srcval = srcsampler.getValue(vit);
		vit.setValue( vit.getValue() + srcval );
	}
}

const SIM_DopDescription *GAS_VDB_TEST::getDopDescription()
{
	static PRM_Name DestField(GAS_NAME_FIELDDEST, "Dest Field");
	static PRM_Name SourceField(GAS_NAME_FIELDSOURCE, "Source Field");

	static std::array<PRM_Template, 3> PRMS{
			PRM_Template(PRM_STRING, 1, &DestField),
			PRM_Template(PRM_STRING, 1, &SourceField),
			PRM_Template()
	};

	static SIM_DopDescription DESC(true,
								   "VDD_TEST_SOLVER",
								   "VDB TEST SOLVER",
								   "VDB_TEST",
								   classname(),
								   PRMS.data());
	setGasDescription(DESC);
	return &DESC;
}
