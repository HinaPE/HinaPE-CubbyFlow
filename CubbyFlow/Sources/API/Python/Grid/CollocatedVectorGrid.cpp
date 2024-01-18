// This code is based on Jet framework.
// Copyright (c) 2018 Doyub Kim
// CubbyFlow is voxel-based fluid simulation engine for computer games.
// Copyright (c) 2020 CubbyFlow Team
// Core Part: Chris Ohk, Junwoo Hwang, Jihong Sin, Seungwoo Yoo
// AI Part: Dongheon Cho, Minseo Kim
// We are making my contributions/submissions to this project solely in our
// personal capacity and are not conveying any rights to any intellectual
// property of any third parties.

#include <API/Python/Grid/CollocatedVectorGrid.hpp>
#include <API/Python/Utils/pybind11Utils.hpp>
#include <Core/Grid/CollocatedVectorGrid.hpp>

#include <pybind11/pybind11.h>

using namespace CubbyFlow;

void AddCollocatedVectorGrid2(pybind11::module& m)
{
    pybind11::class_<CollocatedVectorGrid2, CollocatedVectorGrid2Ptr,
                     VectorGrid2>(static_cast<pybind11::handle>(m),
                                  "CollocatedVectorGrid2",
                                  R"pbdoc(
			Abstract base class for 2-D collocated vector grid structure.
		)pbdoc")
        .def(
            "__getitem__",
            [](const CollocatedVectorGrid2& instance, pybind11::object obj)
                -> Vector2D { return instance(ObjectToVector2UZ(obj)); },
            R"pbdoc(
			Returns the grid data at given data point.

			Parameters
			----------
			- idx : Data point index (i, j).
		)pbdoc",
            pybind11::arg("idx"))
        .def(
            "__setitem__",
            [](CollocatedVectorGrid2& instance, pybind11::object obj,
               const Vector2D& val) { instance(ObjectToVector2UZ(obj)) = val; },
            R"pbdoc(
			Sets the grid data at given data point.

			Parameters
			----------
			- idx : Data point index (i, j).
			- val : Value to set.
		)pbdoc",
            pybind11::arg("idx"), pybind11::arg("val"))
        .def("DivergenceAtDataPoint",
             CUBBYFLOW_PYTHON_MAKE_INDEX_FUNCTION2(CollocatedVectorGrid2,
                                                   DivergenceAtDataPoint),
             R"pbdoc(
			Returns divergence at data point location.

			Parameters
			----------
            - `*args` : Data point index (i, j).
		)pbdoc")
        .def("CurlAtDataPoint",
             CUBBYFLOW_PYTHON_MAKE_INDEX_FUNCTION2(CollocatedVectorGrid2,
                                                   CurlAtDataPoint),
             R"pbdoc(
			Returns curl at data point location.

			Parameters
			----------
            - `*args` : Data point index (i, j).
		)pbdoc")
        .def("DataView",
             static_cast<ArrayView2<Vector2D> (CollocatedVectorGrid2::*)()>(
                 &CollocatedVectorGrid2::DataView),
             R"pbdoc(
			Returns the data array view.
		)pbdoc")
        .def("DataPosition", &CollocatedVectorGrid2::DataPosition,
             R"pbdoc(
			Returns the function that maps data point to its position.
		)pbdoc")
        .def(
            "ForEachDataPointIndex",
            [](CollocatedVectorGrid2& instance, pybind11::function func) {
                instance.ForEachDataPointIndex(func);
            },
            R"pbdoc(
			Invokes the given function `func` for each data point.

			This function invokes the given function object `func` for each data
			point in serial manner. The input parameters are i and j indices of a
			data point. The order of execution is i-first, j-last.
		)pbdoc",
            pybind11::arg("func"))
        .def(
            "Sample",
            [](const CollocatedVectorGrid2& instance, pybind11::object obj) {
                return instance.Sample(ObjectToVector2D(obj));
            },
            R"pbdoc(
			Returns sampled value at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Divergence",
            [](const CollocatedVectorGrid2& instance, pybind11::object obj) {
                return instance.Divergence(ObjectToVector2D(obj));
            },
            R"pbdoc(
			Returns divergence at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Curl",
            [](const CollocatedVectorGrid2& instance, pybind11::object obj) {
                return instance.Curl(ObjectToVector2D(obj));
            },
            R"pbdoc(
			Returns curl at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Sampler",
            [](const CollocatedVectorGrid2& instance) {
                return instance.Sampler();
            },
            R"pbdoc(
			Returns the sampler function.

			This function returns the data sampler function object. The sampling
			function is linear.
		)pbdoc");
}

void AddCollocatedVectorGrid3(pybind11::module& m)
{
    pybind11::class_<CollocatedVectorGrid3, CollocatedVectorGrid3Ptr,
                     VectorGrid3>(static_cast<pybind11::handle>(m),
                                  "CollocatedVectorGrid3",
                                  R"pbdoc(
			Abstract base class for 3-D collocated vector grid structure.
		)pbdoc")
        .def(
            "__getitem__",
            [](const CollocatedVectorGrid3& instance, pybind11::object obj)
                -> Vector3D { return instance(ObjectToVector3UZ(obj)); },
            R"pbdoc(
			Returns the grid data at given data point.

			Parameters
			----------
			- idx : Data point index (i, j, k).
		)pbdoc",
            pybind11::arg("idx"))
        .def(
            "__setitem__",
            [](CollocatedVectorGrid3& instance, pybind11::object obj,
               const Vector3D& val) { instance(ObjectToVector3UZ(obj)) = val; },
            R"pbdoc(
			Sets the grid data at given data point.

			Parameters
			----------
			- idx : Data point index (i, j, k).
			- val : Value to set.
		)pbdoc",
            pybind11::arg("idx"), pybind11::arg("val"))
        .def("DivergenceAtDataPoint",
             CUBBYFLOW_PYTHON_MAKE_INDEX_FUNCTION3(CollocatedVectorGrid3,
                                                   DivergenceAtDataPoint),
             R"pbdoc(
			Returns divergence at data point location.

			Parameters
			----------
            - `*args` : Data point index (i, j, k).
		)pbdoc")
        .def("CurlAtDataPoint",
             CUBBYFLOW_PYTHON_MAKE_INDEX_FUNCTION3(CollocatedVectorGrid3,
                                                   CurlAtDataPoint),
             R"pbdoc(
			Returns curl at data point location.

			Parameters
			----------
            - `*args` : Data point index (i, j, k).
		)pbdoc")
        .def("DataView",
             static_cast<ArrayView3<Vector3D> (CollocatedVectorGrid3::*)()>(
                 &CollocatedVectorGrid3::DataView),
             R"pbdoc(
			Returns the data array view.
		)pbdoc")
        .def("DataPosition", &CollocatedVectorGrid3::DataPosition,
             R"pbdoc(
			Returns the function that maps data point to its position.
		)pbdoc")
        .def(
            "ForEachDataPointIndex",
            [](CollocatedVectorGrid3& instance, pybind11::function func) {
                instance.ForEachDataPointIndex(func);
            },
            R"pbdoc(
			Invokes the given function `func` for each data point.

			This function invokes the given function object `func` for each data
			point in serial manner. The input parameters are i and j indices of a
			data point. The order of execution is i-first, j-last.
		)pbdoc",
            pybind11::arg("func"))
        .def(
            "Sample",
            [](const CollocatedVectorGrid3& instance, pybind11::object obj) {
                return instance.Sample(ObjectToVector3D(obj));
            },
            R"pbdoc(
			Returns sampled value at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Divergence",
            [](const CollocatedVectorGrid3& instance, pybind11::object obj) {
                return instance.Divergence(ObjectToVector3D(obj));
            },
            R"pbdoc(
			Returns divergence at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Curl",
            [](const CollocatedVectorGrid3& instance, pybind11::object obj) {
                return instance.Curl(ObjectToVector3D(obj));
            },
            R"pbdoc(
			Returns curl at given position `x`.
		)pbdoc",
            pybind11::arg("x"))
        .def(
            "Sampler",
            [](const CollocatedVectorGrid3& instance) {
                return instance.Sampler();
            },
            R"pbdoc(
			Returns the sampler function.

			This function returns the data sampler function object. The sampling
			function is linear.
		)pbdoc");
}