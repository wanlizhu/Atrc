#include <Atrc/Material/BxDF/BxDFScaler.h>

AGZ_NS_BEG(Atrc)

BxDFScaler::BxDFScaler(BxDFType type, const Spectrum &scale, RC<BxDF> bxdf)
    : BxDF(type), scale_(scale), bxdf_(std::move(bxdf))
{
    AGZ_ASSERT(bxdf_);
}

Spectrum BxDFScaler::Eval(const Vec3 &wi, const Vec3 &wo) const
{
    return scale_ * bxdf_->Eval(wi, wo);
}

Option<BxDFSampleWiResult> BxDFScaler::SampleWi(const Vec3 &wo, BxDFType type) const
{
    Option<BxDFSampleWiResult> ret = bxdf_->SampleWi(wo, type);
    if(ret)
        ret->coef *= scale_;
    return ret;
}

AGZ_NS_END(Atrc)
