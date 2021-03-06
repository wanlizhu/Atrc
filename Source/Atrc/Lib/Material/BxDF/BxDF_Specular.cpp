#include <Atrc/Lib/Material/BxDF/BxDF_Specular.h>

namespace Atrc
{

namespace
{
    // 计算折射方向向量，发生全反射时返回std::nullopt
    std::optional<Vec3> GetRefractDirection(const Vec3 &wo, const Vec3 &nor, Real eta)
    {
        Real cosThetaI = Abs(wo.z);
        Real sinThetaI2 = Max(Real(0), 1 - cosThetaI * cosThetaI);
        Real sinThetaT2 = eta * eta * sinThetaI2;
        if(sinThetaT2 >= 1)
            return std::nullopt;
        Real cosThetaT = Sqrt(1 - sinThetaT2);
        return (eta * cosThetaI - cosThetaT) * nor - eta * wo;
    }
}

BxDF_Specular::BxDF_Specular(const Spectrum &rc, const Dielectric *fresnel)
    : BxDF(BSDFType(BSDF_SPECULAR | BSDF_TRANSMISSION | BSDF_REFLECTION)),
      rc_(rc), fresnel_(fresnel)
{
    AGZ_ASSERT(fresnel_);
}

Spectrum BxDF_Specular::GetBaseColor() const noexcept
{
    return rc_;
}

Spectrum BxDF_Specular::Eval(
    [[maybe_unused]] const Vec3 &wi,
    [[maybe_unused]] const Vec3 &wo,
    [[maybe_unused]] bool star) const noexcept
{
    return Spectrum();
}

std::optional<BxDF_Specular::SampleWiResult> BxDF_Specular::SampleWi(
    const Vec3 &wo, bool star, const Vec3 &sample) const noexcept
{
    Vec3 nor = wo.z > 0 ? Vec3::UNIT_Z() : -Vec3::UNIT_Z();
    Vec3 nWo = wo.Normalize();

    auto Fr = fresnel_->Eval(nWo.z);
    if(sample.x < Fr.r)
    {
        SampleWiResult ret;
        ret.wi   = Vec3(-nWo.xy(), nWo.z).Normalize();
        ret.coef = rc_ * Fr / Abs(ret.wi.z);
        ret.pdf  = Fr.r;
        ret.type = BSDFType(BSDF_SPECULAR | BSDF_REFLECTION);
        ret.isDelta = true;

        if(ret.coef.HasInf())
            return std::nullopt;
        return ret;
    }

    // 如果是由内向外，etaI和etaT要反过来

    Real etaI = nWo.z > 0 ? fresnel_->GetEtaI() : fresnel_->GetEtaT();
    Real etaT = nWo.z > 0 ? fresnel_->GetEtaT() : fresnel_->GetEtaI();

    Real eta = etaI / etaT;
    auto wi = GetRefractDirection(nWo, nor, eta);

    if(!wi)
        return std::nullopt;
    
    SampleWiResult ret;
    ret.wi = wi->Normalize();
    ret.pdf = 1 - Fr.r;
    ret.coef = (star ? Real(1) : eta * eta) * rc_ * (Spectrum(1.0f) - Fr) / Abs(ret.wi.z);
    ret.type = BSDFType(BSDF_SPECULAR | BSDF_TRANSMISSION);
    ret.isDelta = true;

    if(ret.coef.HasInf())
        return std::nullopt;
    return ret;
}

Real BxDF_Specular::SampleWiPDF(
    [[maybe_unused]] const Vec3 &wi,
    [[maybe_unused]] const Vec3 &wo,
    [[maybe_unused]] bool star) const noexcept
{
    return 0;
}

} // namespace Atrc
