#include "Save.hpp"
#include "FileUtils.hpp"
#include "SettingsJson.hpp"
#include "ACharacter.hpp"

// -- Constructors -------------------------------------------------------------

Save::Save() {
	// TODO(ebaudet): init members
}

Save::Save(int member): _member(member) {
	// TODO(ebaudet): init other members
}

Save::~Save() {
}

Save::Save(Save const &src) {
	*this = src;
}

// -- Operators ----------------------------------------------------------------

Save &Save::operator=(Save const &rhs) {
	if ( this != &rhs ) {
		// TODO(ebaudet): copy each members
		_member = rhs._member;
	}
	return *this;
}

std::ostream &	operator<<(std::ostream & os, const Save& my_class) {
	os << my_class.getMember();
	return (os);
}

// -- Accessors ----------------------------------------------------------------

int		Save::getMember() const { return _member; }
void	Save::setMember(int member) { _member = member; }

// -- Methods ------------------------------------------------------------------

bool	Save::initJson() {
	/*
	Json file should be like this.
	{
		"name": "Toto",
		"date": 1585765499,
		"levels": [
			{
				"id": 1,
				"score": 1500
			},
			{
				"id": 2,
				"score": 700
			}
		],
		"state": {
			"life": 2,
			"bombs": 2,
			"flame": 3,
			"speed": 3.0,
			"wallpass": 0,
			"detonator": 1,
			"bombpass": 0,
			"flampass": 0,
			"shield": 0
		}
	}
	*/
	int				i = 0;
	std::string		saveName = "save" + std::to_string(i);
	// $(savePath)/$(saveName).json
	std::string		filename = s.s("savePath") + "/" + saveName + ".json";
	if (file::isFile(filename) == false) {
		return false;  // file does not exist
	}

	SettingsJson	*save = new SettingsJson();
	save->name(saveName).description("Save file");
	save->add<std::string>(saveName + "Filename", filename);

	// Save json definition
	save->add<std::string>("name");
	save->add<int64_t>("date", 0).setMin(-1).setMax(86400);

	SettingsJson	*levelPattern = new SettingsJson();
	levelPattern->add<int64_t>("id", 0).setMin(0);  // Todo(ebaudet): add setMax to nb levels.
	levelPattern->add<int64_t>("score", 0).setMin(0).setMax(99999999);
	save->addList<SettingsJson>("levels", levelPattern);

	save->add<SettingsJson>("state");
		save->j("state").add<uint64_t>("life", 2).setMin(1).setMax(99);
		save->j("state").add<uint64_t>("bombs", 1).setMin(1).setMax(99);
		save->j("state").add<uint64_t>("flame", 1).setMin(1).setMax(99);
		save->j("state").add<float_t>("speed", 3.0).setMin(3.0).setMax(MAX_SPEED);
		save->j("state").add<uint64_t>("wallpass", 0).setMin(0).setMax(1);
		save->j("state").add<uint64_t>("detonator", 0).setMin(0).setMax(1);
		save->j("state").add<uint64_t>("bombpass", 0).setMin(0).setMax(1);
		save->j("state").add<uint64_t>("flampass", 0).setMin(0).setMax(1);
		save->j("state").add<uint64_t>("shield", 0).setMin(0).setMax(1);

	try {
		if (save->loadFile(filename) == false) {
			logErr("Fail loading " << filename);
			return true;
		}
	} catch (SettingsJson::SettingsException const & e) {
		logErr(e.what());
		return false;
	}

	return true;
}

// -- Exceptions errors --------------------------------------------------------

Save::SaveException::SaveException()
: std::runtime_error("Save Exception") {}

Save::SaveException::SaveException(const char* what_arg)
: std::runtime_error(std::string(std::string("SaveError: ") + what_arg).c_str()) {}
