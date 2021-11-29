#include "config.hpp"

#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

Config::Config() :
	keys{}
{
	std::ifstream     file{CONF_PATH};
	std::stringstream sstream{};
	sstream << file.rdbuf();

	auto const js = nlohmann::json::parse(sstream.str());
	for(size_t i=0; i<KEY_COUNT; i++) {
		auto const key = std::string{static_cast<char>('0' + i)};
		if(js.contains(key)) {
			//std::cout << i << ':' << js[key] << '\n';
			keys[i] = std::make_pair(macro_type_from_str(js[key]["click"]["action"]), js[key]["click"]["data"]);
		} else {
			keys[i] = std::make_pair(MacroType::Text, "Default macro");
		}
	}
};

std::array<std::pair<MacroType, std::string>, KEY_COUNT> keys;

auto
Config::macro_type_from_str(std::string const& str) -> MacroType
{
	if(str == "text")     return MacroType::Text;
	if(str == "sequence") return MacroType::Sequence;

	throw new std::runtime_error{"bad string value"};
}
