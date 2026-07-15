#pragma once

#include <string>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

/// Utility helpers used throughout the mod.
namespace KitsUtils {

    /// Show an error popup using FLAlertLayer.
    inline void showError(const std::string& title, const std::string& message) {
        FLAlertLayer::create(title.c_str(), message.c_str(), "OK")->show();
    }

    /// Show an info popup using FLAlertLayer.
    inline void showInfo(const std::string& title, const std::string& message) {
        FLAlertLayer::create(title.c_str(), message.c_str(), "OK")->show();
    }

    /// Convert a string to lowercase for case-insensitive comparisons.
    inline std::string toLower(std::string str) {
        for (auto& c : str) {
            c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        }
        return str;
    }

    /// Get a formatted timestamp string from epoch seconds.
    inline std::string formatTimestamp(int64_t timestamp) {
        if (timestamp <= 0) return "Unknown";
        auto time = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(timestamp));
        auto tt = std::chrono::system_clock::to_time_t(time);
        std::tm tm{};
#ifdef _WIN32
        localtime_s(&tm, &tt);
#else
        localtime_r(&tt, &tm);
#endif
        char buf[64];
        std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &tm);
        return std::string(buf);
    }

    /// Get current epoch timestamp in seconds.
    inline int64_t currentTimestamp() {
        return static_cast<int64_t>(
            std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::system_clock::now().time_since_epoch()
            ).count()
        );
    }

} // namespace KitsUtils
