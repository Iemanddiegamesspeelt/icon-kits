#include "KitManager.hpp"
#include "Utils.hpp"
#include <fstream>
#include <algorithm>

KitManager& KitManager::get() {
    static KitManager instance;
    return instance;
}

std::filesystem::path KitManager::getSavePath() const {
    auto dir = Mod::get()->getSaveDir();
    return dir / "kits.json";
}

void KitManager::loadFromDisk() {
    auto path = getSavePath();

    if (!std::filesystem::exists(path)) {
        log::info("No kits file found, starting fresh");
        m_kits.clear();
        return;
    }

    try {
        std::ifstream file(path);
        if (!file.is_open()) {
            log::warn("Could not open kits file for reading");
            m_kits.clear();
            return;
        }

        std::string content((std::istreambuf_iterator<char>(file)),
                            std::istreambuf_iterator<char>());
        file.close();

        if (content.empty()) {
            log::info("Kits file is empty, starting fresh");
            m_kits.clear();
            return;
        }

        auto parseResult = matjson::parse(content);
        if (parseResult.isErr()) {
            log::warn("Failed to parse kits JSON: {}", parseResult.unwrapErr());
            m_kits.clear();
            KitsUtils::showError("Icon Kits", "Your saved kits file was corrupted and could not be loaded. Starting fresh.");
            return;
        }

        auto json = parseResult.unwrap();
        m_kits.clear();

        if (json.isArray()) {
            for (auto const& item : json.asArray().unwrap()) {
                m_kits.push_back(Kit::fromJson(item));
            }
        } else if (json.isObject() && json.contains("kits") && json["kits"].isArray()) {
            for (auto const& item : json["kits"].asArray().unwrap()) {
                m_kits.push_back(Kit::fromJson(item));
            }
        } else {
            log::warn("Unexpected kits JSON structure, starting fresh");
        }

        log::info("Loaded {} kits from disk", m_kits.size());
    }
    catch (std::exception const& e) {
        log::error("Exception loading kits: {}", e.what());
        m_kits.clear();
        KitsUtils::showError("Icon Kits", "An error occurred loading your kits. Starting fresh.");
    }
    catch (...) {
        log::error("Unknown exception loading kits");
        m_kits.clear();
        KitsUtils::showError("Icon Kits", "An unexpected error occurred loading your kits. Starting fresh.");
    }
}

void KitManager::saveToDisk() {
    try {
        auto path = getSavePath();
        auto dir = path.parent_path();

        if (!std::filesystem::exists(dir)) {
            std::filesystem::create_directories(dir);
        }

        auto arr = matjson::Value::array();
        for (auto const& kit : m_kits) {
            arr.push(kit.toJson());
        }

        std::ofstream file(path);
        if (!file.is_open()) {
            log::error("Could not open kits file for writing");
            return;
        }
        file << arr.dump(matjson::TAB_INDENTATION);
        file.close();

        log::debug("Saved {} kits to disk", m_kits.size());
    }
    catch (std::exception const& e) {
        log::error("Exception saving kits: {}", e.what());
    }
    catch (...) {
        log::error("Unknown exception saving kits");
    }
}

std::vector<Kit> const& KitManager::getKits() const {
    return m_kits;
}

Kit* KitManager::getKit(size_t index) {
    if (index < m_kits.size()) return &m_kits[index];
    return nullptr;
}

size_t KitManager::count() const {
    return m_kits.size();
}

void KitManager::createKit(const std::string& name) {
    Kit kit;
    kit.name = name.empty() ? "New Kit" : name;
    kit.captureFromGame();
    m_kits.push_back(std::move(kit));
    saveToDisk();
}

void KitManager::addKit(Kit kit) {
    m_kits.push_back(std::move(kit));
    saveToDisk();
}

void KitManager::updateKit(size_t index) {
    if (index >= m_kits.size()) return;
    m_kits[index].captureFromGame();
    saveToDisk();
}

void KitManager::deleteKit(size_t index) {
    if (index >= m_kits.size()) return;
    m_kits.erase(m_kits.begin() + static_cast<ptrdiff_t>(index));
    saveToDisk();
}

void KitManager::renameKit(size_t index, const std::string& newName) {
    if (index >= m_kits.size()) return;
    if (newName.empty()) return;
    m_kits[index].name = newName;
    saveToDisk();
}

void KitManager::duplicateKit(size_t index) {
    if (index >= m_kits.size()) return;
    Kit copy = m_kits[index];
    copy.name += " (Copy)";
    copy.createdAt = KitsUtils::currentTimestamp();
    m_kits.insert(m_kits.begin() + static_cast<ptrdiff_t>(index) + 1, std::move(copy));
    saveToDisk();
}

void KitManager::moveKit(size_t index, bool up) {
    if (index >= m_kits.size()) return;
    if (up) {
        if (index > 0) {
            std::swap(m_kits[index], m_kits[index - 1]);
            saveToDisk();
        }
    } else {
        if (index < m_kits.size() - 1) {
            std::swap(m_kits[index], m_kits[index + 1]);
            saveToDisk();
        }
    }
}

void KitManager::toggleFavorite(size_t index) {
    if (index >= m_kits.size()) return;
    m_kits[index].favorite = !m_kits[index].favorite;
    saveToDisk();
}

void KitManager::loadKit(size_t index) {
    if (index >= m_kits.size()) return;
    m_kits[index].applyToGame();
}

int KitManager::getRandomKitIndex() {
    if (m_kits.empty()) return -1;
    std::uniform_int_distribution<size_t> dist(0, m_kits.size() - 1);
    return static_cast<int>(dist(m_rng));
}

std::vector<size_t> KitManager::search(const std::string& query) const {
    std::vector<size_t> results;
    if (query.empty()) {
        for (size_t i = 0; i < m_kits.size(); ++i)
            results.push_back(i);
        return results;
    }

    auto lowerQuery = KitsUtils::toLower(query);
    for (size_t i = 0; i < m_kits.size(); ++i) {
        auto lowerName = KitsUtils::toLower(m_kits[i].name);
        if (lowerName.find(lowerQuery) != std::string::npos) {
            results.push_back(i);
        }
    }
    return results;
}

void KitManager::sort(SortMode mode) {
    m_sortMode = mode;
    switch (mode) {
        case SortMode::Name:
            std::sort(m_kits.begin(), m_kits.end(), [](Kit const& a, Kit const& b) {
                return KitsUtils::toLower(a.name) < KitsUtils::toLower(b.name);
            });
            break;
        case SortMode::DateNewest:
            std::sort(m_kits.begin(), m_kits.end(), [](Kit const& a, Kit const& b) {
                return a.createdAt > b.createdAt;
            });
            break;
        case SortMode::DateOldest:
            std::sort(m_kits.begin(), m_kits.end(), [](Kit const& a, Kit const& b) {
                return a.createdAt < b.createdAt;
            });
            break;
        case SortMode::FavoritesFirst:
            std::sort(m_kits.begin(), m_kits.end(), [](Kit const& a, Kit const& b) {
                if (a.favorite != b.favorite) return a.favorite;
                return KitsUtils::toLower(a.name) < KitsUtils::toLower(b.name);
            });
            break;
    }
    saveToDisk();
}

SortMode KitManager::getSortMode() const {
    return m_sortMode;
}

void KitManager::cycleSort() {
    switch (m_sortMode) {
        case SortMode::Name: sort(SortMode::DateNewest); break;
        case SortMode::DateNewest: sort(SortMode::DateOldest); break;
        case SortMode::DateOldest: sort(SortMode::FavoritesFirst); break;
        case SortMode::FavoritesFirst: sort(SortMode::Name); break;
    }
}

std::string KitManager::getSortModeName() const {
    switch (m_sortMode) {
        case SortMode::Name: return "A-Z";
        case SortMode::DateNewest: return "Newest";
        case SortMode::DateOldest: return "Oldest";
        case SortMode::FavoritesFirst: return "Favorites";
    }
    return "A-Z";
}

geode::Result<size_t> KitManager::importFromJson(const std::string& jsonStr) {
    if (jsonStr.empty()) {
        return geode::Err("Clipboard is empty. Copy a JSON kit export first.");
    }

    try {
        auto parseResult = matjson::parse(jsonStr);
        if (parseResult.isErr()) {
            return geode::Err("Invalid JSON: " + parseResult.unwrapErr().message);
        }

        auto json = parseResult.unwrap();
        size_t imported = 0;

        if (json.isArray()) {
            for (auto const& item : json.asArray().unwrap()) {
                auto kit = Kit::fromJson(item);
                if (kit.name.empty()) kit.name = "Imported Kit";
                m_kits.push_back(std::move(kit));
                imported++;
            }
        } else if (json.isObject()) {
            if (json.contains("kits") && json["kits"].isArray()) {
                for (auto const& item : json["kits"].asArray().unwrap()) {
                    auto kit = Kit::fromJson(item);
                    if (kit.name.empty()) kit.name = "Imported Kit";
                    m_kits.push_back(std::move(kit));
                    imported++;
                }
            } else {
                auto kit = Kit::fromJson(json);
                if (kit.name.empty()) kit.name = "Imported Kit";
                m_kits.push_back(std::move(kit));
                imported = 1;
            }
        } else {
            return geode::Err("JSON must be an object or array of kit objects.");
        }

        if (imported == 0) {
            return geode::Err("No valid kits found in the JSON data.");
        }

        saveToDisk();
        return geode::Ok(imported);
    }
    catch (std::exception const& e) {
        return geode::Err(std::string("Import failed: ") + e.what());
    }
    catch (...) {
        return geode::Err("An unexpected error occurred during import.");
    }
}

std::string KitManager::exportKit(size_t index) const {
    if (index >= m_kits.size()) return "{}";
    return m_kits[index].toJson().dump(matjson::TAB_INDENTATION);
}

std::string KitManager::exportAllKits() const {
    auto arr = matjson::Value::array();
    for (auto const& kit : m_kits) {
        arr.push(kit.toJson());
    }
    return arr.dump(matjson::TAB_INDENTATION);
}
