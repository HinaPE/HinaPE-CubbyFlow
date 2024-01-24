#include "HOW_TO_USE_GENERATOR.h"

// Solver Implementation
NEW_CUBBY_MICRPSOLVER_IMPLEMENT(
		NEW_SOLVER,
		false,
		NEW_INT_PARAMETER(MyInt, 1) \
        NEW_FLOAT_PARAMETER(MyFloat, 3.14) \
        NEW_STRING_PARAMETER(MyString, "Hello") \
        NEW_BOOL_PARAMETER(MyBool, true) \
        NEW_INT_VECTOR_PARAMETER(MyVector3I, 3, 1, 2, 3) \
        NEW_FLOAT_VECTOR_PARAMETER(MyVector3F, 3, 1., 2., 3.) \
)

void GAS_CF_NEW_SOLVER::_init()
{
	// Implement Your [initializeSubclass] Here

	this->local_int = 0;
	this->local_string = "Hello";
}

void GAS_CF_NEW_SOLVER::_makeEqual(const GAS_CF_NEW_SOLVER *src)
{
	// Implement Your [makeEqualSubclass] Here

	this->local_int = src->local_int;
	this->local_string = src->local_string;
}

bool GAS_CF_NEW_SOLVER::_solve(SIM_Engine &engine, SIM_Object *obj, SIM_Time time, SIM_Time timestep, UT_WorkBuffer &error_msg) const
{
	if (!obj)
	{
		error_msg.appendSprintf("Object Is Null, From %s\n", DATANAME);
		return false;
	}

	// Implement Your Solver Here
//	int64 i = getMyInt();
//	fpreal f = getMyFloat();
//	UT_StringHolder str = getMyString();
//	bool b = getMyBool();
//	UT_Vector3 v3f = getMyVector3F();
//	UT_Vector3I v3i = getMyVector3I();

	return true;
}

void GAS_CF_NEW_SOLVER::_test_function() {}

// Data Implementation
NEW_CUBBY_DATA_IMPLEMENT(
		NEW_DATA,
		false,
		NEW_INT_PARAMETER(MyInt, 1) \
        NEW_FLOAT_PARAMETER(MyFloat, 3.14) \
        NEW_STRING_PARAMETER(MyString, "Hello") \
        NEW_BOOL_PARAMETER(MyBool, true) \
        NEW_INT_VECTOR_PARAMETER(MyVector3I, 3, 1, 2, 3) \
        NEW_FLOAT_VECTOR_PARAMETER(MyVector3F, 3, 1., 2., 3.) \
)

void SIM_CF_NEW_DATA::_init()
{
	// Implement Your [initializeSubclass] Here

	this->local_int = 0;
	this->local_string = "Hello";
}

void SIM_CF_NEW_DATA::_makeEqual(const SIM_CF_NEW_DATA *src)
{
	// Implement Your [makeEqualSubclass] Here

	this->local_int = src->local_int;
	this->local_string = src->local_string;
}

void SIM_CF_NEW_DATA::_test_function() {}
