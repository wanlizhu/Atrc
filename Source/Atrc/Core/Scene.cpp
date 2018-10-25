#include <Atrc/Core/Scene.h>

AGZ_NS_BEG(Atrc)

const Camera* Scene::GetCamera() const
{
    AGZ_ASSERT(camera);
    return camera;
}

const std::vector<const Light*> &Scene::GetLights() const
{
    return lights_;
}

const std::vector<const Entity*> &Scene::GetEntities() const
{
    return entities_;
}

const AABB &Scene::GetWorldBound() const
{
    if(!worldBound_)
    {
        AABB wb;
        for(auto ent : GetEntities())
            wb = wb | ent->WorldBound();
        worldBound_ = wb;
    }
    return *worldBound_;
}

SceneLightSampleResult Scene::SampleLight() const
{
    if(lights_.empty())
        return { nullptr, 0.0 };

    return {
        lights_[AGZ::Math::Random::Uniform<size_t>(0, lights_.size() - 1)],
        1.0 / lights_.size()
    };
}

Real Scene::SampleLightPDF(const Light *light) const
{
    AGZ_ASSERT(light && lights_.size());
    return 1.0 / lights_.size();
}

bool Scene::HasIntersection(const Ray &r) const
{
    AGZ_ASSERT(r.IsNormalized());
    for(auto ent : entities_)
    {
        if(ent->HasIntersection(r))
            return true;
    }
    return false;
}

bool Scene::FindCloestIntersection(const Ray &r, SurfacePoint *sp) const
{
    bool ret = false;
    for(auto ent : entities_)
    {
        SurfacePoint newSp;
        if(ent->FindIntersection(r, &newSp) && (!ret || newSp.t < sp->t))
        {
            ret = true;
            *sp = newSp;
        }
    }
    return ret;
}

AGZ_NS_END(Atrc)