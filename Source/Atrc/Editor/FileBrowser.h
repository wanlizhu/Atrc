#pragma once

#include <filesystem>

#include <Atrc/Editor/GL.h>

class FileBrowser : public AGZ::Uncopiable
{
public:

    FileBrowser() = default;

    FileBrowser(std::string label, bool selectDirectory, std::string_view currentDirectory);

    FileBrowser(FileBrowser&&) noexcept = default;

    FileBrowser &operator=(FileBrowser&&) noexcept = default;

    void SetLabel(std::string label);

    void SetTarget(bool selectDirectory);

    void SetCurrentDirectory(std::string_view dir = "");

    bool Display();

    const std::string &GetLabel() const noexcept;

    std::string GetResult(bool relative = true) const;

private:

    void UpdateCurrentUnits();

    std::string label_ = "default file browser";
    bool selectDirectory_ = false;

    std::string selectedName_;
    std::filesystem::path pwd_;

    struct Unit
    {
        bool isDir;
        std::string name;
    };

    std::vector<Unit> curUnits_;
};
