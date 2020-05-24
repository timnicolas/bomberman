#ifndef STATS_HPP_
#define STATS_HPP_
#include <unordered_map>
#include <iostream>
#include <string>
#include <chrono>

/**
 * @brief sStat element (to store the stats of a function call)
 */
struct sStat {
	int	nbCalls;  ///< Number of function call
	std::chrono::duration<double>	totalExecTime;  ///< Total execution time
	std::chrono::duration<double>	avgExecTime;  ///< Average execution time
	std::chrono::duration<double>	minExecTime;  ///< Min execution time
	std::chrono::duration<double>	maxExecTime;  ///< Max execution time
};

/**
 * @brief Stats object to manage functions calls stats
 */
class Stats {
	public:
		Stats();
		~Stats();
		static std::chrono::high_resolution_clock::time_point	startStats(std::string name);
		static void	endStats(std::string name, std::chrono::high_resolution_clock::time_point startExecTime);
		static void	printStats();

		// Members
		static std::unordered_map<std::string, struct sStat>	stats;  ///< Stats are stored here
};

// -- getStats for clasic functions --------------------------------------------

template<typename RetT, typename ...Args>
RetT getStats(std::string name, RetT (&func)(Args...), Args... args) {
	std::chrono::high_resolution_clock::time_point startExecTime = Stats::startStats(name);
	RetT res = func(args...);
    Stats::endStats(name, startExecTime);
	return res;
}
template<typename ...Args>
void getStatsVoid(std::string name, void (&func)(Args...), Args... args) {
	std::chrono::high_resolution_clock::time_point startExecTime = Stats::startStats(name);
	func(args...);
    Stats::endStats(name, startExecTime);
}


// -- getStats for member function ---------------------------------------------

template<typename RetT, typename ClassT, typename ...Args>
RetT getStatsM(std::string name, ClassT &obj, RetT (ClassT::*func)(Args...), Args... args) {
	std::chrono::high_resolution_clock::time_point startExecTime = Stats::startStats(name);
	RetT res = (obj.*func)(args...);
    Stats::endStats(name, startExecTime);
	return res;
}
template<typename ClassT, typename ...Args>
void getStatsMVoid(std::string name, ClassT &obj, void (ClassT::*func)(Args...), Args... args) {
	std::chrono::high_resolution_clock::time_point startExecTime = Stats::startStats(name);
	(obj.*func)(args...);
    Stats::endStats(name, startExecTime);
}

#endif  // STATS_HPP_
