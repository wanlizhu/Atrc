#pragma once

#include <Atrc/Lib/Material/Utility/BxDF.h>

namespace Atrc
{

class BxDF_Diffuse : public BxDF
{
    Spectrum albedo_;

public:

    explicit BxDF_Diffuse(const Spectrum &albedo) noexcept;

    Spectrum GetAlbedo() const noexcept override;

    Spectrum Eval(const Vec3 &wi, const Vec3 &wo, bool star) const noexcept override;

    std::optional<SampleWiResult> SampleWi(const Vec3 &wo, bool star, const Vec3 &sample) const noexcept override;

    Real SampleWiPDF(const Vec3 &wi, const Vec3 &wo, bool star) const noexcept override;
};

} // namespace Atrc
