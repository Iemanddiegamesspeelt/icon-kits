#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

/// Represents a single icon kit preset containing all cosmetic options.
struct Kit {
    std::string name;

    // Icon IDs
    int cube = 1;
    int ship = 1;
    int ball = 1;
    int ufo = 1;
    int wave = 1;
    int robot = 1;
    int spider = 1;
    int swing = 1;
    int jetpack = 1;

    // Colors
    int primaryColor = 0;
    int secondaryColor = 3;
    int glowColor = 0;

    // Effects
    bool glow = false;
    int deathEffect = 1;
    int streak = 1;
    int shipFire = 1;

    // Metadata
    bool favorite = false;
    int64_t createdAt = 0;

    /// Capture the current player setup from GameManager.
    void captureFromGame();

    /// Apply this kit's settings to GameManager.
    void applyToGame() const;

    /// Serialize to JSON.
    matjson::Value toJson() const;

    /// Deserialize from JSON. Returns a default kit on any parse error.
    static Kit fromJson(matjson::Value const& json);
};
