#pragma once

#include <map>
#include <string>
#include "json.hpp"
#include "Logging.hpp"

namespace JsonOpt {
	/**
	 * @brief options list to print SettingsJson object
	 *
	 * Options:
	 *  - NO_OPT -> default value (just print the json)
	 *  - DISCARD_DISABLED -> dont print values disard in file (used to print the variable in a file)
	 *  - VERBOSE -> verbose mode: print description, min, max, ...
	 *  - COLOR -> enable color to print
	 */
	enum Enum {
		NO_OPT = 0,
		DISCARD_DISABLED = 1,
		VERBOSE = 2,
		COLOR = 4,
	};
}  // namespace JsonOpt

/**
 * @brief object to save an element in SettingsJson (dont use this)
 *
 * @tparam T the object type
 */
template<class T>
class JsonObj {
	public:
		/**
		 * @brief Construct a new Json Obj object
		 */
		JsonObj() : _value() { init(); }
		/**
		 * @brief Construct a new Json Obj object
		 *
		 * @param name The json object name
		 */
		explicit JsonObj(std::string const & name) : _value() { init(name); }
		/**
		 * @brief Construct a new Json Obj object
		 *
		 * @param name The json object name
		 * @param val The json object value
		 */
		JsonObj(std::string const & name, T const & val) : _value(val), _defVal(val) { init(name); }
		/**
		 * @brief Destroy the Json Obj object
		 */
		virtual ~JsonObj() {}
		/**
		 * @brief Construct a new Json Obj object
		 *
		 * @param src The object to do the copy
		 */
		JsonObj(JsonObj const & src) { *this = src; }
		/**
		 * @brief Copy this object
		 *
		 * @param rhs The opjet to copy
		 * @return JsonObj& A reference to the copied object
		 */
		JsonObj & operator=(JsonObj const & rhs) {
			if (this != &rhs) {
				// logDebug("WARNING -> JsonObj object copied");
				_name = rhs._name;
				_description = rhs._description;
				_value = rhs._value;
				_hasMin = rhs._hasMin;
				_min = rhs._min;
				_hasMax = rhs._hasMax;
				_max = rhs._max;
				_disableInFile = rhs._disableInFile;
			}
			return *this;
		}
		/**
		 * @brief Init a json object
		 *
		 * @param name The json object name
		 */
		void init(std::string const & name = "") {
			_name = name;
			_description = "";
			_hasMin = false;
			_hasMax = false;
			_disableInFile = false;
		}

		/**
		 * @brief Get json object value
		 *
		 * @return T const& The value
		 */
		T const &		get() const { return _value; }
		/**
		 * @brief Get json object value
		 *
		 * @return T const& The value
		 */
		T &				get() { return _value; }
		/**
		 * @brief Set the Value object
		 *
		 * @param value The new value
		 * @return JsonObj<T>& A reference to 'this' object
		 */
		JsonObj<T> &	setValue(T value) {
			if (_hasMin && value < _min) {
				logWarn("unable to set arg " << _name << ": " << value << " is less than min value (" << _min << ")");
				return *this;
			}
			if (_hasMax && value > _max) {
				logWarn("unable to set arg " << _name << ": " << value << " is greater than max value (" << _max << ")");
				return *this;
			}
			_value = value;
			return *this;
		}
		/**
		 * @brief Check is a value is in min < value < max
		 *
		 * @param value The value
		 * @return true If min < value < max
		 */
		bool			checkValue(T value) const {
			if (_hasMin && value < _min) {
				return false;
			}
			if (_hasMax && value > _max) {
				return false;
			}
			return true;
		}
		/**
		 * @brief Set the Min object
		 *
		 * @param value The min value
		 * @return JsonObj<T>& A reference to 'this' object
		 */
		JsonObj<T> &		setMin(T value) { _hasMin = true; _min = value; return *this; }
		/**
		 * @brief Set the Max object
		 *
		 * @param value The max value
		 * @return JsonObj<T>& A reference to 'this' object
		 */
		JsonObj<T> &		setMax(T value) { _hasMax = true; _max = value; return *this; }
		/**
		 * @brief Get the Max object
		 *
		 * @return T& The max value
		 */
		T &					getMax() { return _max; }
		/**
		 * @brief Get the Max object
		 *
		 * @return T& The max value
		 */
		T const &			getMax() const { return _max; }
		/**
		 * @brief Get the Min object
		 *
		 * @return T& The min value
		 */
		T &					getMin() { return _min; }
		/**
		 * @brief Get the Min object
		 *
		 * @return T& The min value
		 */
		T const &			getMin() const { return _min; }
		/**
		 * @brief Set the Description object
		 *
		 * @param desc The description
		 * @return JsonObj<T>& A reference to 'this' object
		 */
		JsonObj<T> &		setDescription(std::string const & desc) { _description = desc; return *this; }
		/**
		 * @brief Disable modification of this value on json files
		 *
		 * @param disable Disable option
		 * @return JsonObj<T>& A reference to 'this' object
		 */
		JsonObj<T> &		disableInFile(bool disable = true) { _disableInFile = disable; return *this; }
		/**
		 * @brief Know if this value is disabled in file
		 *
		 * @return true If value is disabled in file
		 */
		bool				isDisabledInFile() { return _disableInFile; }
		/**
		 * @brief Set the Def Val object
		 *
		 * @param val The default value
		 */
		void				setDefVal(T val) { _defVal = val; }
		/**
		 * @brief Get the Def Val object
		 *
		 * @return T The default value
		 */
		T					getDefVal() const { return _defVal; }
		/**
		 * @brief Reset value
		 */
		void				reset() { _value = _defVal; }
		/**
		 * @brief Set the Name object
		 *
		 * @param name The name
		 */
		void				setName(std::string const & name) { _name = name; }
		/**
		 * @brief Get the Name object
		 *
		 * @return std::string& The name
		 */
		std::string	&		getName() { return _name; }
		/**
		 * @brief Get the Name object
		 *
		 * @return std::string& The name
		 */
		std::string	const &	getName() const { return _name; }
		/**
		 * @brief Get informations about object
		 *
		 * @return std::string The info
		 */
		std::string			getInfo() const {
			std::ostringstream out;
			out << "  // ";
			if (_description != "")
				out << _description << " ";
			out << "<";
			if (typeid(T) == typeid(std::string)) out << "string";
			else if (typeid(T) == typeid(int64_t)) out << "int64";
			else if (typeid(T) == typeid(uint64_t)) out << "uint64";
			else if (typeid(T) == typeid(double)) out << "double";
			else if (typeid(T) == typeid(bool)) out << "bool";
			if (_hasMin || _hasMax) {
				out << " range=[";
				if (_hasMin)
					out << _min;
				out << ":";
				if (_hasMax)
					out << _max;
				out << "]";
			}
			if (_disableInFile) {
				out << " [disabled in file]";
			}
			out << ">";
			return out.str();
		}

		/**
		 * @brief Cout operator
		 *
		 * @param out The ostream object
		 * @param jsonObj The jsonObj to cout
		 * @return std::ostream& The ostream obj
		 */
		friend std::ostream & operator<<(std::ostream & out, const JsonObj & jsonObj) {
			out << std::boolalpha << jsonObj.get();
			return out;
		}

	protected:
		std::string _name;  /**< The name of the object */
		std::string _description;  /**< The description of the object */
		T			_value;  /**< The object value */
		bool		_hasMin;  /**< If the object has a minimum value */
		T			_min;  /**< The minimum value (if _hasMin) */
		bool		_hasMax;  /**< If the object has a maximum value */
		T			_max;  /**< The maximum value (if _hasMax) */
		bool		_disableInFile;  /**< An option to disable this parameter in json files */
		T			_defVal;  /**< The default value */
};

/**
 * @brief this is the object to save json list object (dont use this)
 *
 * @tparam T the list type (only SettingsJson for now)
 */
template<class T>
class SettingsList {
	public:
		/**
		 * @brief Construct a new Settings List object
		 */
		SettingsList() : pattern(nullptr) {}
		/**
		 * @brief Construct a new Settings List object
		 *
		 * @param pattern_ The base pattern for list
		 */
		explicit SettingsList(T * pattern_) : pattern(pattern_) {}
		/**
		 * @brief Destroy the Settings List object
		 */
		virtual ~SettingsList() {
			resetList();
			delete pattern;
		}
		/**
		 * @brief Construct a new Settings List object
		 *
		 * @param src The object to do the copy
		 */
		SettingsList(SettingsList const & src) { *this = src; }
		/**
		 * @brief Copy this object
		 *
		 * @param rhs The opjet to copy
		 * @return SettingsList& A reference to the copied object
		 */
		SettingsList & operator=(SettingsList const & rhs) {
			if (this != &rhs) {
				// logDebug("WARNING -> SettingsList object copied");
				pattern = new T(*rhs.pattern);
				for (auto it = list.begin(); it != list.end(); it++) {
					T * tmp = new T(**it);
					list.push_back(tmp);
				}
			}
			return *this;
		}
		/**
		 * @brief Set the Pattern object
		 *
		 * @param pattern_ The new pattern
		 */
		void setPattern(T * pattern_) {
			if (pattern != nullptr) {
				logFatal("unable to add pattern in list -> there is already a pattern");
			}
			pattern = pattern_;
		}
		/**
		 * @brief Add a new object in list
		 *
		 * @param newObj The new object
		 */
		void add(T * newObj) {
			list.push_back(newObj);
		}
		/**
		 * @brief Reset the list (delete all elements)
		 */
		void resetList() {
			for (auto it = list.begin(); it != list.end(); it++) {
				delete *it;
			}
			list.clear();
		}

		/**
		 * @brief Cout operator
		 *
		 * @param out The ostream object
		 * @param settingsList The settingsList to cout
		 * @return std::ostream& The ostream obj
		 */
		friend std::ostream & operator<<(std::ostream & out, const SettingsList & settingsList) {
			out << "[" << std::endl;
			for (auto it = settingsList.list.begin(); it != settingsList.list.end(); it++) {
				bool termWithComma = true;
				if (it + 1 == settingsList.list.end()) {
					termWithComma = false;
				}
				out << (*it)->toString(JsonOpt::NO_OPT, 2, termWithComma);
			}
			out << "\t]";
			return out;
		}

		T *					pattern;  /**< The default SettingsJson object for list */
		std::vector<T *>	list;  /**< A list of pattern */
};

/**
 * @brief this is the settings master object
 *
 * With this object, you can create a json pattern, load files, save files, ...
 */
class SettingsJson {
	public:
		SettingsJson();
		SettingsJson(SettingsJson const &src);
		virtual ~SettingsJson();

		SettingsJson &operator=(SettingsJson const &rhs);

		SettingsJson &	name(std::string const & name);
		SettingsJson &	description(std::string const & description);

		bool		loadFile(std::string const & filename);
		bool		loadJson(nlohmann::json const & json, SettingsJson & jsonObjTmp);
		void		saveToFile(std::string const & filename);
		std::string	toString(uint32_t opt = JsonOpt::NO_OPT, uint32_t tabOffset = 0, bool termWithComma = false) const;

		/**
		 * @brief Add new object
		 *
		 * @tparam T The object type
		 * @param name The object name
		 * @return JsonObj<T>& A reference to the created object
		 */
		template<class T>
		JsonObj<T> &	add(std::string const & name) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return *(tmpMap[name]);
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T> *>(name, new JsonObj<T>(name)));
			return *(tmpMap[name]);
		}
		/**
		 * @brief Add new object
		 *
		 * @tparam T The object type
		 * @param name The object name
		 * @param val The object value
		 * @return JsonObj<T>& A reference to the created object
		 */
		template<class T>
		JsonObj<T> &	add(std::string const & name, T val) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return *(tmpMap[name]);
			}
			tmpMap.insert(std::pair<std::string, JsonObj<T> *>(name, new JsonObj<T>(name, val)));
			return *(tmpMap[name]);
		}
		/**
		 * @brief Add a new list
		 *
		 * @tparam T The list type (generally SettingsJson)
		 * @param name The list name
		 * @param val The list pattern
		 * @return JsonObj<SettingsList<T> >& A reference to the created list
		 */
		template<class T>
		JsonObj<SettingsList<T> > &	addList(std::string const & name, T * val) {
			std::map<std::string, JsonObj<SettingsList<T> > *> & tmpMap = _getMap<SettingsList<T> >();
			if (tmpMap.find(name) != tmpMap.end()) {
				logWarn("cannot add setting " << name << ": setting already exist");
				return *(tmpMap[name]);
			}
			tmpMap.insert(std::pair<std::string, JsonObj<SettingsList<T> > *>(name,
				new JsonObj<SettingsList<T> >(name)));
			tmpMap[name]->get().setPattern(val);
			return *(tmpMap[name]);
		}
		/**
		 * @brief Update a value
		 *
		 * @tparam T The value type
		 * @param name The value name
		 * @return JsonObj<T>& A reference to the object (to update)
		 */
		template<class T>
		JsonObj<T> &	update(std::string const & name)  {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end()) {
				return *(tmpMap[name]);
			}
			throw SettingsException("undefined setting " + name);
		}
		/**
		 * @brief Get a value
		 *
		 * @tparam T The value type
		 * @param name The value name
		 * @return T const& The value
		 */
		template<class T>
		T const &		get(std::string const & name) const {
			std::map<std::string, JsonObj<T> *> const & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name)->get();
			throw SettingsException("undefined setting " + name);
		}
		/**
		 * @brief Get a value
		 *
		 * @tparam T The value type
		 * @param name The value name
		 * @return T & The value
		 */
		template<class T>
		T &				get(std::string const & name) {
			std::map<std::string, JsonObj<T> *> & tmpMap = _getMap<T>();
			if (tmpMap.find(name) != tmpMap.end())
				return tmpMap.at(name)->get();
			throw SettingsException("undefined setting " + name);
		}
		/**
		 * @brief Get a string value
		 *
		 * @param name The name
		 * @return std::string const& The value
		 */
		std::string const &		s(std::string const & name) const { return get<std::string>(name); }
		/**
		 * @brief Get a string value
		 *
		 * @param name The name
		 * @return std::string & The value
		 */
		std::string &			s(std::string const & name)  { return get<std::string>(name); }
		/**
		 * @brief Get a int value
		 *
		 * @param name The name
		 * @return int64_t const& The value
		 */
		int64_t const &			i(std::string const & name) const { return get<int64_t>(name); }
		/**
		 * @brief Get a int value
		 *
		 * @param name The name
		 * @return int64_t & The value
		 */
		int64_t &				i(std::string const & name)  { return get<int64_t>(name); }
		/**
		 * @brief Get a uint value
		 *
		 * @param name The name
		 * @return uint64_t const& The value
		 */
		uint64_t const &		u(std::string const & name) const { return get<uint64_t>(name); }
		/**
		 * @brief Get a uint value
		 *
		 * @param name The name
		 * @return uint64_t & The value
		 */
		uint64_t &				u(std::string const & name)  { return get<uint64_t>(name); }
		/**
		 * @brief Get a double value
		 *
		 * @param name The name
		 * @return double const& The value
		 */
		double const &			d(std::string const & name) const { return get<double>(name); }
		/**
		 * @brief Get a double value
		 *
		 * @param name The name
		 * @return double & The value
		 */
		double &				d(std::string const & name)  { return get<double>(name); }
		/**
		 * @brief Get a bool value
		 *
		 * @param name The name
		 * @return bool const& The value
		 */
		bool const &			b(std::string const & name) const { return get<bool>(name); }
		/**
		 * @brief Get a bool value
		 *
		 * @param name The name
		 * @return bool & The value
		 */
		bool &					b(std::string const & name)  { return get<bool>(name); }
		/**
		 * @brief Get a json value
		 *
		 * @param name The name
		 * @return SettingsJson const& The value
		 */
		SettingsJson const &	j(std::string const & name) const { return get<SettingsJson>(name); }
		/**
		 * @brief Get a json value
		 *
		 * @param name The name
		 * @return SettingsJson & The value
		 */
		SettingsJson &			j(std::string const & name)  { return get<SettingsJson>(name); }
		/**
		 * @brief Get a json list value
		 *
		 * @param name The name
		 * @return SettingsList<SettingsJson> const& The value
		 */
		SettingsList<SettingsJson> const &	lj(std::string const & name) const {
			return get<SettingsList<SettingsJson>>(name);
		}
		/**
		 * @brief Get a json list value
		 *
		 * @param name The name
		 * @return SettingsList<SettingsJson> & The value
		 */
		SettingsList<SettingsJson> &		lj(std::string const & name)  { return get<SettingsList<SettingsJson>>(name); }

		class SettingsException : public std::runtime_error {
			public:
				SettingsException();
				explicit SettingsException(const char* what_arg);
				explicit SettingsException(const std::string what_arg);
		};

		std::map<std::string, JsonObj<std::string> *>	stringMap;
		/**< A map with all string objects (fn s) */
		std::map<std::string, JsonObj<int64_t> *>		intMap;
		/**< A map with all int64_t objects (fn i) */
		std::map<std::string, JsonObj<uint64_t> *>		uintMap;
		/**< A map with all uint64_t objects (fn u) */
		std::map<std::string, JsonObj<double> *>		doubleMap;
		/**< A map with all double objects (fn f) */
		std::map<std::string, JsonObj<bool> *>			boolMap;
		/**< A map with all bool objects (fn b) */
		std::map<std::string, JsonObj<SettingsJson> *>	jsonMap;
		/**< A map with all SettingsJson objects (fn j) */
		std::map<std::string, JsonObj<SettingsList<SettingsJson> > *>	jsonList;
		/**< A map with all SettingsList objects (fn j) */

	private:
		std::string _name;  /**< The json name */
		std::string _description;  /**< The json description */

		/**
		 * @brief Get the right map for a type
		 *
		 * @tparam T The map type
		 * @return std::map<std::string, JsonObj<T> *> const& The map
		 */
		template<class T>
		std::map<std::string, JsonObj<T> *> const & _getMap() const {
			if (typeid(T) == typeid(int64_t)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&intMap);
				return *ptr;
			}
			if (typeid(T) == typeid(uint64_t)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&uintMap);
				return *ptr;
			}
			if (typeid(T) == typeid(double)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&doubleMap);
				return *ptr;
			}
			if (typeid(T) == typeid(bool)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&boolMap);
				return *ptr;
			}
			if (typeid(T) == typeid(std::string)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&stringMap);
				return *ptr;
			}
			if (typeid(T) == typeid(SettingsJson)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&jsonMap);
				return *ptr;
			}
			if (typeid(T) == typeid(SettingsList<SettingsJson>)) {
				std::map<std::string, JsonObj<T> *> const *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> const *>(&jsonList);
				return *ptr;
			}
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}
		/**
		 * @brief Get the right map for a type
		 *
		 * @tparam T The map type
		 * @return std::map<std::string, JsonObj<T> *> const& The map
		 */
		template<class T>
		std::map<std::string, JsonObj<T> *> & _getMap() {
			if (typeid(T) == typeid(int64_t)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&intMap);
				return *ptr;
			}
			if (typeid(T) == typeid(uint64_t)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&uintMap);
				return *ptr;
			}
			if (typeid(T) == typeid(double)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&doubleMap);
				return *ptr;
			}
			if (typeid(T) == typeid(bool)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&boolMap);
				return *ptr;
			}
			if (typeid(T) == typeid(std::string)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&stringMap);
				return *ptr;
			}
			if (typeid(T) == typeid(SettingsJson)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&jsonMap);
				return *ptr;
			}
			if (typeid(T) == typeid(SettingsList<SettingsJson>)) {
				std::map<std::string, JsonObj<T> *> *__attribute__((__may_alias__)) ptr = \
					reinterpret_cast<std::map<std::string, JsonObj<T> *> *>(&jsonList);
				return *ptr;
			}
			throw SettingsException(std::string("invalid type ") + typeid(T).name());
		}

		/**
		 * @brief Delete a map
		 *
		 * @tparam T The type
		 * @param map The map
		 */
		template<class T>
		void _deleteMap(std::map<std::string, JsonObj<T> *> & map) {
			for (auto it = map.begin(); it != map.end(); it++) {
				delete it->second;
			}
			map.erase(map.begin(), map.end());
		}

		/**
		 * @brief Copy a map
		 *
		 * @tparam T The type
		 * @param dest The destination
		 * @param src The src map
		 * @return std::map<std::string, JsonObj<T> *>& The created map (reference)
		 */
		template<class T>
		std::map<std::string, JsonObj<T> *> & _copyMap(std::map<std::string, JsonObj<T> *> & dest,
			std::map<std::string, JsonObj<T> *> const & src)
		{
			for (auto it = src.begin(); it != src.end(); it++) {
				dest.insert(std::pair<std::string, JsonObj<T> *>(it->first, new JsonObj<T>(*it->second)));
			}
			return dest;
		}
};

std::ostream & operator<<(std::ostream & o, SettingsJson const & s);
