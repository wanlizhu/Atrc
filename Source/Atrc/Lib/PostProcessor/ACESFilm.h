#pragma once

#include <Atrc/Lib/Core/PostProcessor.h>

namespace Atrc
{
    
class ACESFilm : public PostProcessor
{
public:

    void Process(Image *image) const noexcept override;
};

} // namespace Atrc
