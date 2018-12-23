#pragma once

#include <Atrc/Lib/Core/Light.h>

namespace Atrc
{

class SkyLight : public Light
{
    Spectrum top_;
    Spectrum bottom_;

    Vec3 worldCentre_;
    Real worldRadius_;

public:

    explicit SkyLight(const Spectrum &topAndBottom);

    SkyLight(const Spectrum &top, const Spectrum &bottom);

    void PreprocessScene(const Scene &scene) override;

    SampleWiResult SampleWi(const Intersection &inct, const ShadingPoint &shd, const Vec3 &sample) const noexcept override;

    Real SampleWiAreaPDF(const Vec3 &pos, const Vec3 &nor, const Intersection &inct, const ShadingPoint &shd) const noexcept override;

    Real SampleWiNonAreaPDF(const Vec3 &wi, const Intersection &inct, const ShadingPoint &shd) const noexcept override;

    SampleWiResult SampleWi(const Vec3 &medPos, const Vec3 &sample) const noexcept override;

    Real SampleWiAreaPDF(const Vec3 &pos, const Vec3 &nor, const Vec3 &medPos) const noexcept override;

    Real SampleWiNonAreaPDF(const Vec3 &wi, const Vec3 &medPos) const noexcept override;

    Spectrum AreaLe(const Intersection &inct) const noexcept override;

    Spectrum NonAreaLe(const Ray &r) const noexcept override;
};

} // namespace Atrc
