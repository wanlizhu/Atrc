#pragma once

#include <Atrc/Lib/Core/Common.h>
#include <Atrc/Lib/Core/SurfacePoint.h>

namespace Atrc
{

enum BSDFType
{
    BSDF_NONSPECULAR  = (1 << 0),
    BSDF_SPECULAR      = (1 << 1),

    BSDF_REFLECTION   = (1 << 2),
    BSDF_TRANSMISSION = (1 << 3),

    BSDF_NONE = 0,
    BSDF_NULL = (1 << 4),
    BSDF_ALL  = BSDF_NONSPECULAR | BSDF_SPECULAR | BSDF_REFLECTION | BSDF_TRANSMISSION
};

bool Contains(BSDFType set, BSDFType subset) noexcept;

class BSDF
{
public:

    struct SampleWiResult
    {
        Vec3 wi;
        Real pdf;
        Spectrum coef;
        BSDFType type;
        bool isDelta;
    };

    virtual ~BSDF() = default;

    virtual Spectrum GetBaseColor(BSDFType type) const noexcept = 0;

    virtual Spectrum Eval(
        const CoordSystem &shd, const CoordSystem &geo,
        const Vec3 &wi, const Vec3 &wo, BSDFType type, bool star) const noexcept = 0;

    virtual std::optional<SampleWiResult> SampleWi(
        const CoordSystem &shd, const CoordSystem &geo,
        const Vec3 &wo, BSDFType type, bool star, const Vec3 &sample) const noexcept = 0;

    virtual Real SampleWiPDF(
        const CoordSystem &shd, const CoordSystem &geo,
        const Vec3 &wi, const Vec3 &wo, BSDFType type, bool star) const noexcept = 0;
};

class Material
{
public:

    virtual ~Material() = default;

    virtual ShadingPoint GetShadingPoint(const Intersection &inct, Arena &arena) const = 0;
};

// ================================= Implementation

inline bool Contains(BSDFType set, BSDFType subset) noexcept
{
    return (subset & set) == subset;
}

} // namespace Atrc
