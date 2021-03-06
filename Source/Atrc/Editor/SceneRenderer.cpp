#include <Atrc/Editor/FilmRTReporter.h>
#include <Atrc/Editor/SceneRenderer.h>
#include <Atrc/Lib/Core/Renderer.h>
#include <Atrc/Mgr/BuiltinCreatorRegister.h>
#include <Atrc/Mgr/Parser.h>
#include <Atrc/Mgr/SceneBuilder.h>
#include "Global.h"

SceneRenderer::SceneRenderer()
    : renderer_(nullptr), sampler_(nullptr), filmFilter_(nullptr), saved_(false)
{
    
}

bool SceneRenderer::Start(const AGZ::Config &config, std::string_view configPath)
{
	Clear();
	AGZ::ScopeGuard clearGuard([&]{ Clear(); });
	
	try
	{
		auto &root = config.Root();
		context_ = std::make_unique<Atrc::Mgr::Context>(root, configPath);
		Atrc::Mgr::RegisterBuiltinCreators(*context_);
		
        std::string outputFilename = context_->GetPathInWorkspace(root["outputFilename"].AsValue());
		
		renderer_   = context_->Create<Atrc::Renderer>(root["renderer"]);
		sampler_    = context_->Create<Atrc::Sampler> (root["sampler"]);
		filmFilter_ = context_->Create<Atrc::FilmFilter>(root["film.filter"]);
		
		scene_ = std::make_unique<Atrc::Scene>();
		*scene_ = Atrc::Mgr::SceneBuilder::Build(root, *context_);
		
		Vec2i filmSize = Atrc::Mgr::Parser::ParseVec2i(root["film.size"]);
		film_ = std::make_unique<Atrc::Film>(filmSize, *filmFilter_);
        outputFilename_ = context_->GetPathInWorkspace(root["outputFilename"].AsValue());
		reporter_ = std::make_unique<FilmRTReporter>();
		
		renderer_->Render(scene_.get(), sampler_, film_.get(), reporter_.get());

        Global::ShowNormalMessage("start rendering");
	}
	catch(...)
	{
		return false;
	}
	
	clearGuard.Dismiss();
	return true;
}

void SceneRenderer::Stop()
{
	AGZ_ASSERT(renderer_);
	renderer_->Stop();
}

bool SceneRenderer::IsRendering() const
{
    return renderer_ != nullptr;
}

bool SceneRenderer::IsCompleted() const
{
    AGZ_ASSERT(IsRendering());
    return renderer_->IsCompleted();
}

void SceneRenderer::Join()
{
    AGZ_ASSERT(IsRendering());
    renderer_->Join(reporter_.get());
}

void SceneRenderer::Clear()
{
	context_    = nullptr;
	renderer_   = nullptr;
	sampler_    = nullptr;
	filmFilter_ = nullptr;
	
	scene_    = nullptr;
	reporter_ = nullptr;
	film_     = nullptr;
    outputFilename_ = "";
	
	filmSize_ = Vec2i(0, 0);
    saved_ = false;
}

void SceneRenderer::CheckSaving()
{
    if(IsRendering() && IsCompleted() && !saved_)
    {
        saved_ = true;
        try
        {
            auto img = film_->GetImage();
            AGZ::TextureFile::WriteRGBToPNG(outputFilename_, img.Map(
                [](const Atrc::Spectrum &c)
            {
                return c.Map([](Atrc::Real s)
                { return uint8_t(AGZ::Math::Clamp<Atrc::Real>(s * 256, 0, 255)); });
            }));
            Global::ShowNormalMessage("film image saved to " + outputFilename_);
        }
        catch(...)
        {
            Global::ShowErrorMessage("failed to save film image");
        }
    }
}
