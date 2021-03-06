#pragma once

#include <filesystem>

#include <Atrc/Editor/LauncherScriptExportingContext.h>
#include <Atrc/Editor/FileBrowser.h>
#include <Atrc/Editor/ResourceManagement/ResourceManager.h>

enum class FilenameMode
{
    RelativeToWorkspace,
    RelativeToScript,
    RelativeToCurrentDirectory,
    Absolute,
};

template<bool TCanSelectOutputMode, FilenameMode TDefaultFilenameMode = FilenameMode::RelativeToWorkspace>
class TFilenameSlot
{
    FilenameMode outputMode_ = TDefaultFilenameMode;
    std::string filename_;

    std::string Mode2Icon(FilenameMode mode)
    {
        switch(mode)
        {
        case FilenameMode::RelativeToWorkspace:        return "[ ]";
        case FilenameMode::RelativeToScript:           return "[@]";
        case FilenameMode::RelativeToCurrentDirectory: return "[$]";
        default:                                       return "[a]";
        }
    }

public:

    void Clear()
    {
        filename_ = "";
    }

    void SetFilename(std::string filename) noexcept
    {
        filename_ = std::move(filename);
    }

    bool Display(FileBrowser &fileBrowser)
    {
        bool ret;

        ImGui::BeginChild(fileBrowser.GetLabel().c_str(), ImVec2(0, ImGui::GetTextLineHeight()));

        auto curModeIcon = Mode2Icon(outputMode_);

        if(ImGui::SmallButton("set"))
            ImGui::OpenPopup(fileBrowser.GetLabel().c_str());
        if((ret = fileBrowser.Display()))
            filename_ = fileBrowser.GetResult();

        ImGui::SameLine();
        ImGui::Text("%s", curModeIcon.c_str());

        if constexpr(TCanSelectOutputMode)
        {
            if(ImGui::IsItemClicked(0))
                ImGui::OpenPopup("set output mode");
            if(ImGui::BeginPopup("set output mode", ImGuiWindowFlags_AlwaysAutoResize))
            {
                AGZ::ScopeGuard popupExitGuard([] { ImGui::EndPopup(); });
                if(ImGui::BeginCombo("filename type", curModeIcon.c_str()))
                {
                    struct Icon2Mode
                    {
                        std::string icon;
                        FilenameMode mode = FilenameMode::Absolute;
                    } MODES[] =
                    {
                        { "[ ]", FilenameMode::RelativeToWorkspace },
                        { "[@]", FilenameMode::RelativeToScript },
                        { "[$]", FilenameMode::RelativeToCurrentDirectory },
                        { "[a]", FilenameMode::Absolute }
                    };
                    bool closeCurrentPopup = false;
                    for(size_t i = 0; i < AGZ::ArraySize(MODES); ++i)
                    {
                        ImGui::PushID(static_cast<int>(i));
                        bool selected = outputMode_ == MODES[i].mode;
                        if(ImGui::Selectable(MODES[i].icon.c_str(), selected))
                        {
                            curModeIcon = MODES[i].icon;
                            outputMode_ = MODES[i].mode;
                            closeCurrentPopup = true;
                        }
                        if(selected)
                            ImGui::SetItemDefaultFocus();
                        ImGui::PopID();
                    }
                    ImGui::EndCombo();
                    if(closeCurrentPopup)
                        ImGui::CloseCurrentPopup();
                }
            }
            ImGui::SameLine();
        }

        ImGui::SameLine();

        ImGui::Text("%s", filename_.c_str());
        
        ImGui::ShowTooltipForLastItem(filename_.c_str());

        ImGui::EndChild();

        return ret;
    }

    const std::string &GetFilename() const noexcept
    {
        return filename_;
    }

    std::string GetExportedFilename(std::string_view workspaceDir, std::string_view scriptDir) const
    {
        using std::filesystem::path;
        switch(outputMode_)
        {
        case FilenameMode::RelativeToWorkspace:
            return relative(path(filename_), (path(scriptDir) / path(workspaceDir))).string();
        case FilenameMode::RelativeToScript:
            return relative(path(filename_), absolute(path(scriptDir))).string();
        case FilenameMode::RelativeToCurrentDirectory:
            return relative(path(filename_)).string();
        default:
            return absolute(path(filename_)).string();
        }
    }

    std::string GetExportedFilename(const LauncherScriptExportingContext &ctx) const
    {
        return GetExportedFilename(ctx.workspaceDirectory, ctx.scriptDirectory);
    }

    void Import(std::string_view filename, const IResource::ImportContext &ctx)
    {
        auto absFilename = ctx.GetFilename(filename);
        filename_ = relative(std::filesystem::path(absFilename)).string();
    }
};
