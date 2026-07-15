#pragma once

#include "Kit.hpp"
#include <vector>
#include <string>
#include <optional>
#include <random>

/// Sort mode for the kit list.
enum class SortMode {
    Name,
    DateNewest,
    DateOldest,
    FavoritesFirst
};

/// Singleton manager that owns all kits and handles persistence.
class KitManager {
public:
    static KitManager& get();

    /// Load kits from disk. Called once at startup.
    void loadFromDisk();

    /// Save kits to disk. Called automatically after every mutation.
    void saveToDisk();

    /// Get all kits (const reference).
    std::vector<Kit> const& getKits() const;

    /// Get a mutable reference to a kit by index.
    Kit* getKit(size_t index);

    /// Number of kits.
    size_t count() const;

    /// Create a new kit from the current player setup.
    void createKit(const std::string& name);

    /// Add a kit directly and save to disk.
    void addKit(Kit kit);

    /// Overwrite an existing kit with the current player setup.
    void updateKit(size_t index);

    /// Delete a kit by index.
    void deleteKit(size_t index);

    /// Rename a kit by index.
    void renameKit(size_t index, const std::string& newName);

    /// Duplicate a kit by index.
    void duplicateKit(size_t index);

    /// Move a kit up or down in the list order.
    void moveKit(size_t index, bool up);

    /// Toggle favorite on a kit.
    void toggleFavorite(size_t index);

    /// Load (apply) a kit by index to the current player.
    void loadKit(size_t index);

    /// Get a random kit index, or -1 if no kits exist.
    int getRandomKitIndex();

    /// Search kits by name (case-insensitive). Returns indices into the main list.
    std::vector<size_t> search(const std::string& query) const;

    /// Sort the kits in place.
    void sort(SortMode mode);

    /// Get the current sort mode.
    SortMode getSortMode() const;

    /// Cycle to the next sort mode.
    void cycleSort();

    /// Get a display name for the current sort mode.
    std::string getSortModeName() const;

    /// Import kits from a JSON string. Returns number of kits imported, or error.
    geode::Result<size_t> importFromJson(const std::string& jsonStr);

    /// Export a single kit to JSON string.
    std::string exportKit(size_t index) const;

    /// Export all kits to JSON string.
    std::string exportAllKits() const;

private:
    KitManager() = default;

    std::vector<Kit> m_kits;
    SortMode m_sortMode = SortMode::Name;
    std::mt19937 m_rng{std::random_device{}()};

    std::filesystem::path getSavePath() const;
};
