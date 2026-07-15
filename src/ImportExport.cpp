#include "ImportExport.hpp"
#include "KitManager.hpp"
#include "Utils.hpp"
#include <Geode/utils/general.hpp>

bool ImportExport::exportKitToClipboard(size_t index) {
    try {
        auto json = KitManager::get().exportKit(index);
        if (json.empty() || json == "{}") {
            KitsUtils::showError("Export Error", "Could not export the selected kit.");
            return false;
        }
        geode::utils::clipboard::write(json);
        KitsUtils::showInfo("Exported", "Kit copied to clipboard!");
        return true;
    }
    catch (std::exception const& e) {
        KitsUtils::showError("Export Error", std::string("Failed to export: ") + e.what());
        return false;
    }
    catch (...) {
        KitsUtils::showError("Export Error", "An unexpected error occurred during export.");
        return false;
    }
}

bool ImportExport::exportAllToClipboard() {
    try {
        if (KitManager::get().count() == 0) {
            KitsUtils::showError("Export Error", "No kits to export.");
            return false;
        }
        auto json = KitManager::get().exportAllKits();
        geode::utils::clipboard::write(json);
        KitsUtils::showInfo("Exported", fmt::format("All {} kits copied to clipboard!", KitManager::get().count()));
        return true;
    }
    catch (std::exception const& e) {
        KitsUtils::showError("Export Error", std::string("Failed to export: ") + e.what());
        return false;
    }
    catch (...) {
        KitsUtils::showError("Export Error", "An unexpected error occurred during export.");
        return false;
    }
}

void ImportExport::importFromClipboard() {
    try {
        auto clipText = geode::utils::clipboard::read();
        if (clipText.empty()) {
            KitsUtils::showError("Import Error", "Clipboard is empty. Copy a JSON kit export first.");
            return;
        }

        auto result = KitManager::get().importFromJson(clipText);
        if (result.isOk()) {
            size_t count = result.unwrap();
            KitsUtils::showInfo("Imported", fmt::format("Successfully imported {} kit{}!", count, count == 1 ? "" : "s"));
        } else {
            KitsUtils::showError("Import Error", result.unwrapErr());
        }
    }
    catch (std::exception const& e) {
        KitsUtils::showError("Import Error", std::string("Failed to import: ") + e.what());
    }
    catch (...) {
        KitsUtils::showError("Import Error", "An unexpected error occurred during import.");
    }
}
