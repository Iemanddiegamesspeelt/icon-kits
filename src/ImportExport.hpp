#pragma once

#include "Kit.hpp"
#include <vector>
#include <string>

/// JSON import/export helpers using clipboard.
namespace ImportExport {

    /// Export a single kit to clipboard as JSON. Returns true on success.
    bool exportKitToClipboard(size_t index);

    /// Export all kits to clipboard as JSON. Returns true on success.
    bool exportAllToClipboard();

    /// Import kits from clipboard JSON. Shows result popup.
    void importFromClipboard();

} // namespace ImportExport
