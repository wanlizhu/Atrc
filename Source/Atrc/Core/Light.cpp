#include <Atrc/Core/Light.h>

AGZ_NS_BEG(Atrc)

void Light::PreprocessScene([[maybe_unused]] const Scene &scene)
{
    // do nothing
}

bool Light::IsDelta() const
{
    return IsDeltaPosition() || IsDeltaDirection();
}

bool Light::IgnoreFirstMedium() const
{
    return false;
}

AGZ_NS_END(Atrc)
