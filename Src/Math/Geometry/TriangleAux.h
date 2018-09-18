#pragma once

#include "../AGZMath.h"
#include "../Geometry.h"
#include "../Ray.h"

AGZ_NS_BEG(Atrc::TriangleAux)

bool HasIntersection(const Ray &r, const Vec3r &A, const Vec3r &B, const Vec3r &C);

struct TriangleGeometryIntersection
{
    Real coefA, coefB, coefC;
};

Option<TriangleGeometryIntersection> EvalIntersection(
    const Ray &r, const Vec3r &A, const Vec3r &B, const Vec3r &C);

AGZ_NS_END(Atrc::TriangleAux)
