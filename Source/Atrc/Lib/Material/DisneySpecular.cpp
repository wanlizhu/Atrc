#include <Atrc/Lib/Material/BxDF/BxDF_DisneySpecular.h>
#include <Atrc/Lib/Material/DisneySpecular.h>
#include <Atrc/Lib/Material/Utility/BxDFAggregate.h>
#include <Atrc/Lib/Material/Utility/MaterialHelper.h>

namespace Atrc
{
    
DisneySpecularMaterial::DisneySpecularMaterial(
    const Texture *baseColor,
    const Texture *specular,
    const Texture *specularTint,
    const Texture *metallic,
    const Texture *roughness,
    const Texture *anisotropic,
    const NormalMapper *normalMapper) noexcept
    : baseColor_(baseColor), specular_(specular), specularTint_(specularTint),
      metallic_(metallic), roughness_(roughness), anisotropic_(anisotropic),
      normalMapper_(normalMapper)
{
    
}

ShadingPoint DisneySpecularMaterial::GetShadingPoint(const Intersection &inct, Arena &arena) const
{
    ShadingPoint ret;
    ret.uv = inct.usr.uv;
    ret.coordSys = MatHelper::ComputeShadingCoordSystem(inct, normalMapper_);

    Spectrum baseColor = baseColor_->Sample(ret.uv);
    Real specular = specular_->Sample1(ret.uv);
    Real specularTint = specularTint_->Sample1(ret.uv);
    Real metallic = metallic_->Sample1(ret.uv);
    Real roughness = roughness_->Sample1(ret.uv);
    Real anisotropic = anisotropic_->Sample1(ret.uv);
    BxDF *bxdf = arena.Create<BxDF_DisneySpecular>(baseColor, specular, specularTint, metallic, roughness, anisotropic);

    auto bsdf = arena.Create<BxDFAggregate<1>>(ret.coordSys, inct.coordSys);
    bsdf->AddBxDF(bxdf);
    ret.bsdf = bsdf;

    return ret;
}

} // namespace Atrc
