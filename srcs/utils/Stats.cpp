#include "Stats.hpp"
#include <algorithm>
#include <iomanip>

#include "Logging.hpp"

std::unordered_map<std::string, struct sStat> Stats::stats = {};

/**
 * @brief Construct a new Stats object
 */
Stats::Stats() {
}

/**
 * @brief Destroy a Stats object
 */
Stats::~Stats() {
}

/**
 * @brief Print functions stats, print only the one that has been called
 * with getStats
 */
void    Stats::printStats() {
    if (stats.size() > 0) {
        logDebug("functions calls stats:");
    }

    for (auto const &stat : stats) {
        logDebug(stat.first << "{");
        logDebug("  function called: " << stat.second.nbCalls << " times");
        logDebug("  total exec time: " << std::fixed << std::setprecision(8) <<
            stat.second.totalExecTime.count() << "s");
        logDebug("  average: " << std::fixed << std::setprecision(8) <<
            stat.second.avgExecTime.count() << "s");
        if (stat.second.minExecTime != std::chrono::duration<double>::max()) {
            logDebug("  min: " << std::fixed << std::setprecision(8) <<
                stat.second.minExecTime.count() << "s");
        }
        if (stat.second.maxExecTime != std::chrono::duration<double>::min()) {
            logDebug("  max: " << std::fixed << std::setprecision(8) <<
                stat.second.maxExecTime.count() << "s");
        }
        logDebug("}");
    }
}

/**
 * @brief Called by getStats to init the stats, no need to manually call
 *
 * @param name the function name
 * @return std::chrono::high_resolution_clock::time_point, the start execution time
 */
std::chrono::high_resolution_clock::time_point  Stats::startStats(std::string name) {
    if (Stats::stats.find(name) == Stats::stats.end()) {
        struct sStat stats;
        stats.nbCalls = 0;
        stats.totalExecTime = std::chrono::duration<double>(0);
        stats.avgExecTime = std::chrono::duration<double>(0);
        stats.minExecTime = std::chrono::duration<double>::max();
        stats.maxExecTime = std::chrono::duration<double>::min();
        Stats::stats[name] = stats;
    }
    ++(Stats::stats[name].nbCalls);
    return std::chrono::high_resolution_clock::now();
}

/**
 * @brief Called by getStats to stop the stats count, no need to manually call
 */
void	Stats::endStats(std::string name, std::chrono::high_resolution_clock::time_point \
startExecTime) {
    if (Stats::stats.find(name) == Stats::stats.end()
    || startExecTime == std::chrono::high_resolution_clock::time_point::min())
        return;
    std::chrono::duration<double> execTime = std::chrono::duration_cast<std::chrono::duration<double> >(
        std::chrono::high_resolution_clock::now() - startExecTime);
    Stats::stats[name].totalExecTime += execTime;
    Stats::stats[name].avgExecTime = Stats::stats[name].totalExecTime / Stats::stats[name].nbCalls;
    Stats::stats[name].minExecTime = std::min(Stats::stats[name].minExecTime, execTime);
    Stats::stats[name].maxExecTime = std::max(Stats::stats[name].maxExecTime, execTime);
}
