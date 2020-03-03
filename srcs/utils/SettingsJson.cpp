#include "SettingsJson.hpp"

SettingsJson::SettingsJson() {
}

SettingsJson::SettingsJson(SettingsJson const &src) {
	*this = src;
}

SettingsJson::~SettingsJson() {
	deleteMap<std::string>(stringMap);
	deleteMap<int64_t>(intMap);
	deleteMap<uint64_t>(uintMap);
	deleteMap<double>(doubleMap);
	deleteMap<bool>(boolMap);
	deleteMap<SettingsJson>(jsonMap);
	deleteMap<SettingsList<SettingsJson> >(jsonList);
}

SettingsJson &SettingsJson::operator=(SettingsJson const &rhs) {
	if (this != &rhs) {
		logDebug("WARNING -> SettingsJson object copied");
		_name = rhs._name;
		_description = rhs._description;
		stringMap = copyMap<std::string>(rhs.stringMap);
		intMap = copyMap<int64_t>(rhs.intMap);
		uintMap = copyMap<uint64_t>(rhs.uintMap);
		doubleMap = copyMap<double>(rhs.doubleMap);
		boolMap = copyMap<bool>(rhs.boolMap);
		jsonMap = copyMap<SettingsJson>(rhs.jsonMap);
		jsonList = copyMap<SettingsList<SettingsJson> >(rhs.jsonList);
	}
	return *this;
}

SettingsJson & SettingsJson::name(std::string const & name) {
	_name = name;
	return *this;
}
SettingsJson & SettingsJson::description(std::string const & description) {
	_description = description;
	return *this;
}

bool SettingsJson::loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp) {
	bool ret = true;
	for (auto it = json.begin(); it != json.end(); ++it) {
		if (it->is_object()) {
			auto newJsonObj = jsonObjTmp.jsonMap.find(it.key());
			if (newJsonObj != jsonObjTmp.jsonMap.end()) {
				if (jsonObjTmp.update<SettingsJson>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file");
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
					logWarn("you can't set " << it.key() << " in setting file");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<int64_t>(it.key()).checkValue(it->get<int64_t>());
					jsonObjTmp.update<int64_t>(it.key()).setValue(it->get<int64_t>());
				}
			}
			else if (it->is_number_unsigned() && jsonObjTmp.uintMap.find(it.key()) != jsonObjTmp.uintMap.end()) {
				if (jsonObjTmp.update<uint64_t>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<uint64_t>(it.key()).checkValue(it->get<uint64_t>());
					jsonObjTmp.update<uint64_t>(it.key()).setValue(it->get<uint64_t>());
				}
			}
			else if (it->is_number_float() && jsonObjTmp.doubleMap.find(it.key()) != jsonObjTmp.doubleMap.end()) {
				if (jsonObjTmp.update<double>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<double>(it.key()).checkValue(it->get<double>());
					jsonObjTmp.update<double>(it.key()).setValue(it->get<double>());
				}
			}
			else if (it->is_boolean() && jsonObjTmp.boolMap.find(it.key()) != jsonObjTmp.boolMap.end()) {
				if (jsonObjTmp.update<bool>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<bool>(it.key()).checkValue(it->get<bool>());
					jsonObjTmp.update<bool>(it.key()).setValue(it->get<bool>());
				}
			}
			else if (it->is_string() && jsonObjTmp.stringMap.find(it.key()) != jsonObjTmp.stringMap.end()) {
				if (jsonObjTmp.update<std::string>(it.key()).isDisabledInFile()) {
					logWarn("you can't set " << it.key() << " in setting file");
					ret = false;
				}
				else {
					ret &= jsonObjTmp.update<std::string>(it.key()).checkValue(it->get<std::string>());
					jsonObjTmp.update<std::string>(it.key()).setValue(it->get<std::string>());
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
	catch (const nlohmann::json::parse_error& e) {
		throw SettingsException("invalid file format: " + filename);
	}
	catch (std::exception &e) {
		throw SettingsException("unable to open file: " + filename);
	}
	return true;
}

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
int getSize(std::map<std::string, JsonObj<T> *> const & map, uint32_t opt) {
	int size = 0;
	for (auto it = map.begin(); it != map.end(); it++) {
		if (opt & JsonOpt::DISCARD_DISABLED && it->second->isDisabledInFile())
			continue;
		size++;
	}
	return size;
}

template<class T>
int jsonString(std::ostream & out, std::map<std::string, JsonObj<T> *> const & map, uint32_t opt, int nbTab,
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

int jsonStringRecursiv(std::ostream & out, std::map<std::string, JsonObj<SettingsJson> *> const & map,
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

std::string SettingsJson::toString(uint32_t opt, uint32_t tabOffset) const {
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
	out << "}" << std::endl;
	return out.str();
}

// -- cout --------------------------------------------------------
std::ostream & operator<<(std::ostream & out, SettingsJson const & s) {
	out << s.toString();
	return out;
}

// -- Exceptions errors --------------------------------------------------------
SettingsJson::SettingsException::SettingsException()
: std::runtime_error("[SettingsException]") {}

SettingsJson::SettingsException::SettingsException(const char* what_arg)
: std::runtime_error(std::string(std::string("[SettingsException] ") + what_arg).c_str()) {}

SettingsJson::SettingsException::SettingsException(const std::string what_arg)
: std::runtime_error(std::string(std::string("[SettingsException] ") + what_arg).c_str()) {}
