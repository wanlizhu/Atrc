#include <chrono>

#include <Atrc/Lib/Sampler/NativeSampler.h>
#include <Atrc/Mgr/BuiltinCreator/SamplerCreator.h>

namespace Atrc::Mgr
{

void RegisterBuiltinSamplerCreators(Context &context)
{
    static const NativeSamplerCreator nativeSamplerCreator;
    context.AddCreator(&nativeSamplerCreator);
}

Sampler *NativeSamplerCreator::Create(const ConfigGroup &group, [[maybe_unused]] Context &context, Arena &arena) const
{
    ATRC_MGR_TRY
    {
        int seed;
        if(auto seedNode = group.Find("seed"))
            seed = seedNode->Parse<int>();
        else
            seed = int(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        
        int spp = group["spp"].Parse<int>();
        if(spp <= 0)
            throw MgrErr("Invalid spp value");

        return arena.Create<NativeSampler>(seed, spp);
    }
    ATRC_MGR_CATCH_AND_RETHROW("In creating native sampler: " + group.ToString())
}

} // namespace Atrc::Mgr
