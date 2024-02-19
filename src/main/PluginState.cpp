#include "PluginState.h"

namespace fs = std::filesystem;

fs::path GetProjectDirectory(std::optional<fs::path> append)
{
    fs::path path = std::filesystem::current_path()/PROJECT_NAME_STRICT;

    if ( append.has_value() )
        path /= append.value();

    return path;
}
