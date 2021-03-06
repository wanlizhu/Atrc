#include <Atrc/Editor/ResourceManagement/RendererCreator.h>

namespace
{
    class PathTracingRendererInstance : public RendererInstance
    {
        int workerCount_ = -1;
        int taskGridSize_ = 16;
        PathTracingIntegratorSlot integratorSlot_;

    protected:

        void Export(const ResourceManager &rscMgr, LauncherScriptExportingContext &ctx) const override
        {
            ctx.AddLine("type = PathTracing;");
            ExportSubResource("integrator", rscMgr, ctx, integratorSlot_);
            ctx.AddLine("workerCount = ", workerCount_, ";");
            ctx.AddLine("taskGridSize = ", taskGridSize_, ";");
        }

    public:

        PathTracingRendererInstance(ResourceManager &rscMgr, std::string typeName, std::string name)
            : RendererInstance(std::move(typeName), std::move(name)), workerCount_(-1), taskGridSize_(32)
        {
            integratorSlot_.SetInstance(rscMgr.Create<PathTracingIntegratorInstance>("Full", ""));
        }

        void Display(ResourceManager &rscMgr) override
        {
            ImGui::InputInt("worker count", &workerCount_, 0);
            ImGui::InputInt("task size", &taskGridSize_, 0);
            if(ImGui::TreeNodeEx("integrator", ImGuiTreeNodeFlags_DefaultOpen))
            {
                integratorSlot_.Display(rscMgr);
                ImGui::TreePop();
            }
        }

        void Import(ResourceManager &rscMgr, const AGZ::ConfigGroup &root, const AGZ::ConfigGroup &params, const ImportContext &ctx) override
        {
            workerCount_ = params["workerCount"].Parse<int>();
            taskGridSize_ = params["taskGridSize"].Parse<int>();
            integratorSlot_.SetInstance(GetResourceInstance<PathTracingIntegratorInstance>(rscMgr, root, params["integrator"], ctx));
        }
    };
}

void RegisterRendererCreators(ResourceManager &rscMgr)
{
    static const PathTracingRendererCreator iPathTracingRendererCreator;
    rscMgr.AddCreator(&iPathTracingRendererCreator);
}

std::shared_ptr<RendererInstance> PathTracingRendererCreator::Create(ResourceManager &rscMgr, std::string name) const
{
    return std::make_shared<PathTracingRendererInstance>(rscMgr, GetName(), std::move(name));
}
