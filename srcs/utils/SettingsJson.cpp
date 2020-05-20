#include "SettingsJson.hpp"

/**
 * @brief Construct a new Settings Json:: Settings Json object
 */
SettingsJson::SettingsJson() {
}

/**
 * @brief Construct a new Settings Json:: Settings Json object
 *
 * @param src The object to do the copy
 */
SettingsJson::SettingsJson(SettingsJson const &src) {
	*this = src;
}

/**
 * @brief Destroy the Settings Json:: Settings Json object
 */
SettingsJson::~SettingsJson() {
	_deleteMap<std::string>(stringMap);
	_deleteMap<int64_t>(intMap);
	_deleteMap<uint64_t>(uintMap);
	_deleteMap<double>(doubleMap);
	_deleteMap<bool>(boolMap);
	_deleteMap<SettingsJson>(jsonMap);
	_deleteMap<SettingsList<SettingsJson> >(jsonList);
}

/**
 * @brief Copy this object
 *
 * @param rhs The object to copy
 * @return SettingsJson& A reference to the copied object
 */
SettingsJson &SettingsJson::operator=(SettingsJson const &rhs) {
	if (this != &rhs) {
		// logDebug("WARNING -> SettingsJson object copied");
		_name = rhs._name;
		_description = rhs._description;
		_copyMap<std::string>(stringMap, rhs.stringMap);
		_copyMap<int64_t>(intMap, rhs.intMap);
		_copyMap<uint64_t>(uintMap, rhs.uintMap);
		_copyMap<double>(doubleMap, rhs.doubleMap);
		_copyMap<bool>(boolMap, rhs.boolMap);
		_copyMap<SettingsJson>(jsonMap, rhs.jsonMap);
		_copyMap<SettingsList<SettingsJson> >(jsonList, rhs.jsonList);
	}
	return *this;
}

/**
 * @brief set the name of this SettingsJson (used in toString())
 *
 * @param name the new SettingsJson's name
 * @return SettingsJson& a reference to SettingsJson object
 */
SettingsJson & SettingsJson::name(std::string const & name) {
	_name = name;
	return *this;
}
/**
 * @brief set the description of this SettingsJson (used in toString())
 *
 * @param description the new SettingsJson's description
 * @return SettingsJson& a reference to SettingsJson object
 */
SettingsJson & SettingsJson::description(std::string const & description) {
	_description = description;
	return *this;
}

/**
 * @brief load a nlohmann::json object in current SettingsJson
 *
 * @param json The nlohmann::json object
 * @param jsonObjTmp The current SettingsJson object
 * @return true if success
 * @return false if error
 */
bool SettingsJson::loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp) {
	bool ret = true;
	for (auto it = json.begin(); it != json.end(); ++it) {
		if (it->is_object()) {
			auto newJsonObj = jsonObjTmp.jsonMap.find(it.key());
			if (newJsonObj != jsonObjTmp.jsonMap.end()) {
				if (jsonObjTmp.update<SettingsJson>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= loadJson(*it, newJsonObj->second->get());
				}
			}
			else {
				logWarn("invalid setting: " << it.key());
				ret = false;
			}
		}
		else {
			if (it->is_number_integer() && jsonObjTmp.intMap.find(it.key()) != jsonObjTmp.intMap.end()) {
				if (jsonObjTmp.update<int64_t>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<int64_t>(it.key()).checkValue(it->get<int64_t>());
					jsonObjTmp.update<int64_t>(it.key()).setValue(it->get<int64_t>());
				}
			}
			else if (it->is_number_unsigned() && jsonObjTmp.uintMap.find(it.key()) != jsonObjTmp.uintMap.end()) {
				if (jsonObjTmp.update<uint64_t>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<uint64_t>(it.key()).checkValue(it->get<uint64_t>());
					jsonObjTmp.update<uint64_t>(it.key()).setValue(it->get<uint64_t>());
				}
			}
			else if ((it->is_number_float() || it->is_number_integer())
			&& jsonObjTmp.doubleMap.find(it.key()) != jsonObjTmp.doubleMap.end())
			{
				if (jsonObjTmp.update<double>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<double>(it.key()).checkValue(it->get<double>());
					jsonObjTmp.update<double>(it.key()).setValue(it->get<double>());
				}
			}
			else if (it->is_boolean() && jsonObjTmp.boolMap.find(it.key()) != jsonObjTmp.boolMap.end()) {
				if (jsonObjTmp.update<bool>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<bool>(it.key()).checkValue(it->get<bool>());
					jsonObjTmp.update<bool>(it.key()).setValue(it->get<bool>());
				}
			}
			else if (it->is_string() && jsonObjTmp.stringMap.find(it.key()) != jsonObjTmp.stringMap.end()) {
				if (jsonObjTmp.update<std::string>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<std::string>(it.key()).checkValue(it->get<std::string>());
					jsonObjTmp.update<std::string>(it.key()).setValue(it->get<std::string>());
				}
			}
			else if (it->is_array() && jsonObjTmp.jsonList.find(it.key()) != jsonObjTmp.jsonList.end()) {
				if (jsonObjTmp.update<SettingsList<SettingsJson> >(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file (isDisabledInFile)");
					ret = false;
				}
				else {
					SettingsList<SettingsJson> & tmpList = jsonList[it.key()]->get();
					tmpList.resetList();
					for (auto & it2 : it->items()) {
						SettingsJson * newElem = new SettingsJson(*tmpList.pattern);
						ret &= newElem->loadJson(it2.value(), *newElem);
						tmpList.add(newElem);
					}
				}
			}
			else {
				ret = false;
				logWarn("invalid setting type or name: " << it.key());
			}
		}
	}
	return ret;
}

/**
 * @brief Load a json file in SettingsJson object
 *
 * @param filename The file to load
 * @return true if success
 * @return false if error
 *
 * @throw SettingsException if the loading failed
 */
bool SettingsJson::loadFile(std::string const &filename) {
	try {
		std::ifstream fileStream(filename, std::ifstream::in);

		nlohmann::json	data;
		if (fileStream.is_open()) {
			fileStream >> data;
		}
		else {
			throw SettingsException("invalid file format: " + filename);
		}
		return loadJson(data, *this);
	}
	catch (nlohmann::json::parse_error const & e) {
		throw SettingsException("invalid file format: " + filename);
	}
	catch (std::exception const & e) {
		throw SettingsException("unable to open file: " + filename);
	}
	return true;
}

/**
 * @brief Save the current SettingsJson object in a file
 *
 * @param filename The file to save
 *
 * @throw SettingsException if the saving failed
 */
void SettingsJson::saveToFile(std::string const & filename) {
	std::ofstream settingsFile(filename);
	if (settingsFile.fail()) {
		throw SettingsException("unable to save settings file " + filename + ": " + strerror(errno));
	}
	settingsFile << toString(JsonOpt::DISCARD_DISABLED);
	if (settingsFile.fail()) {
		throw SettingsException("unable to save settings file " + filename + ": " + strerror(errno));
	}
	settingsFile.close();
}

template<class T>
static int getSize(std::map<std::string, JsonObj<T> *> const & map, uint32_t opt) {
	int size = 0;
	for (auto it = map.begin(); it != map.end(); it++) {
		if (opt & JsonOpt::DISCARD_DISABLED && it->second->isDisabledInFile())
			continue;
		size++;
	}
	return size;
}

template<class T>
static int jsonString(std::ostream & out, std::map<std::string, JsonObj<T> *> const & map, uint32_t opt, int nbTab,
int nbElemRemain, std::string const & before = "", std::string const & after = "") {
	for (auto it = map.begin(); it != map.end(); it++) {
		if (opt & JsonOpt::DISCARD_DISABLED && it->second->isDisabledInFile())
			continue;
		nbElemRemain--;
		for (int i = 0; i < nbTab; i++)
			out << "\t";
		out << '"';
		if (opt & JsonOpt::COLOR) out << COLOR_BOLD;
		out << it->first;
		if (opt & JsonOpt::COLOR) out << COLOR_EOC;
		out << "\": " << before;
		out << *(it->second);
		out << after;
		if (nbElemRemain > 0)
			out << ",";
		if (opt & JsonOpt::VERBOSE) {
			if (opt & JsonOpt::COLOR) out << COLOR_DIM;
			out << it->second->getInfo();
			if (opt & JsonOpt::COLOR) out << COLOR_EOC;
		}
		out << std::endl;
	}
	return getSize<T>(map, opt);
}

static int jsonStringRecursiv(std::ostream & out, std::map<std::string, JsonObj<SettingsJson> *> const & map,
uint32_t opt, int nbTab) {
	int nbRem = getSize<SettingsJson>(map, opt);
	for (auto it = map.begin(); it != map.end(); it++) {
		if (opt & JsonOpt::DISCARD_DISABLED && it->second->isDisabledInFile())
			continue;
		nbRem--;
		for (int i = 0; i < nbTab; i++)
			out << "\t";
		if (it->first == "") {
			out << "{" << std::endl;
		}
		else {
			out << '"';
			if (opt & JsonOpt::COLOR) out << COLOR_BOLD;
			out << it->first;
			if (opt & JsonOpt::COLOR) out << COLOR_EOC;
			out << "\": {" << std::endl;
		}
		int nbElem = getSize<std::string>(it->second->get().stringMap, opt)
					+ getSize<uint64_t>(it->second->get().uintMap, opt)
					+ getSize<int64_t>(it->second->get().intMap, opt)
					+ getSize<double>(it->second->get().doubleMap, opt)
					+ getSize<bool>(it->second->get().boolMap, opt)
					+ getSize<SettingsJson>(it->second->get().jsonMap, opt)
					+ getSize<SettingsList<SettingsJson> >(it->second->get().jsonList, opt);
		nbElem -= jsonString<std::string>(out, it->second->get().stringMap, opt, nbTab + 1,
																			nbElem, "\"", "\"");
		nbElem -= jsonString<uint64_t>(out, it->second->get().uintMap, opt, nbTab + 1, nbElem);
		nbElem -= jsonString<int64_t>(out, it->second->get().intMap, opt, nbTab + 1, nbElem);
		nbElem -= jsonString<double>(out, it->second->get().doubleMap, opt, nbTab + 1, nbElem);
		nbElem -= jsonString<bool>(out, it->second->get().boolMap, opt, nbTab + 1, nbElem);
		nbElem -= jsonString<SettingsList<SettingsJson> >(out, it->second->get().jsonList, opt, nbTab + 1, nbElem);
		nbElem -= jsonStringRecursiv(out, it->second->get().jsonMap, opt, nbTab + 1);
		for (int i = 0; i < nbTab; i++)
			out << "\t";
		out << "}";
		if (nbRem > 0)
			out << ",";
		out << std::endl;
	}
	return getSize<SettingsJson>(map, opt);
}

/**
 * @brief Convert SettingsJson to string
 *
 * You can change print options:
 *  - JsonOpt::NO_OPT -> default value (just print the json)
 *  - JsonOpt::DISCARD_DISABLED -> dont print values disard in file (used to print the variable in a file)
 *  - JsonOpt::VERBOSE -> verbose mode: print description, min, max, ...
 *  - JsonOpt::COLOR -> enable color to print
 *
 * @param opt All the option, you can use pipe to set multiple options (JsonOpt::VERBOSE | JsonOpt::COLOR)
 * @param tabOffset Don't use this option
 * @param termWithComma Don't use this option
 * @return std::string The SettingsJson object as string
 */
std::string SettingsJson::toString(uint32_t opt, uint32_t tabOffset, bool termWithComma) const {
	std::ostringstream out;
	for (uint32_t i = 0; i < tabOffset; i++)
		out << "\t";
	if (opt & JsonOpt::VERBOSE) {
		out << "\"";
		if (opt & JsonOpt::COLOR) out << COLOR_BOLD;
		out << _name;
		if (opt & JsonOpt::COLOR) out << COLOR_EOC;
		out << "\": ";
	}
	out << "{";
	if (opt & JsonOpt::VERBOSE) {
		if (opt & JsonOpt::COLOR) out << COLOR_DIM;
		out << "  // " << _description;
		if (opt & JsonOpt::COLOR) out << COLOR_EOC;
	}
	out << std::endl;
	int nbElem = getSize<std::string>(stringMap, opt)
				+ getSize<uint64_t>(uintMap, opt)
				+ getSize<int64_t>(intMap, opt)
				+ getSize<double>(doubleMap, opt)
				+ getSize<bool>(boolMap, opt)
				+ getSize<SettingsJson>(jsonMap, opt)
				+ getSize<SettingsList<SettingsJson> >(jsonList, opt);
	nbElem -= jsonString<std::string>(out, stringMap, opt, tabOffset + 1, nbElem, "\"", "\"");
	nbElem -= jsonString<uint64_t>(out, uintMap, opt, tabOffset + 1, nbElem);
	nbElem -= jsonString<int64_t>(out, intMap, opt, tabOffset + 1, nbElem);
	nbElem -= jsonString<double>(out, doubleMap, opt, tabOffset + 1, nbElem);
	nbElem -= jsonString<bool>(out, boolMap, opt, tabOffset + 1, nbElem);
	nbElem -= jsonString<SettingsList<SettingsJson> >(out, jsonList, opt, tabOffset + 1, nbElem);
	nbElem -= jsonStringRecursiv(out, jsonMap, opt, tabOffset + 1);
	for (uint32_t i = 0; i < tabOffset; i++)
		out << "\t";
	out << "}";
	if (termWithComma)
		out << ",";
	out << std::endl;
	return out.str();
}

// -- cout --------------------------------------------------------
/**
 * @brief Cout operator
 *
 * @param out The ostream object
 * @param s the SettingsJson to cout
 * @return std::ostream& The ostream object
 */
std::ostream & operator<<(std::ostream & out, SettingsJson const & s) {
	out << s.toString();
	return out;
}

// -- Exceptions errors --------------------------------------------------------
/**
 * @brief Construct a new Settings Json:: Settings Exception:: Settings Exception object
 */
SettingsJson::SettingsException::SettingsException()
: std::runtime_error("[SettingsException]") {}

/**
 * @brief Construct a new Settings Json:: Settings Exception:: Settings Exception object
 *
 * @param what_arg Error message
 */
SettingsJson::SettingsException::SettingsException(const char* what_arg)
: std::runtime_error(std::string(std::string("[SettingsException] ") + what_arg).c_str()) {}

/**
 * @brief Construct a new Settings Json:: Settings Exception:: Settings Exception object
 *
 * @param what_arg Error message
 */
SettingsJson::SettingsException::SettingsException(const std::string what_arg)
: std::runtime_error(std::string(std::string("[SettingsException] ") + what_arg).c_str()) {}
