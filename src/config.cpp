#include "config.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

#include <nlohmann/json.hpp>

Config::Config() :
	click_keys{}
{
	reload();
};

void
Config::reload()
{
	std::ifstream     file{get_config_path()};
	std::stringstream sstream{};
	sstream << file.rdbuf();

	auto const js = nlohmann::json::parse(sstream.str());
	for(size_t i=0; i<KEY_COUNT; i++) {
		auto const key = std::string{static_cast<char>('0' + i)};
		if(js.contains(key)) {
			//std::cout << i << ':' << js[key] << '\n';
			click_keys[i] = std::make_pair(macro_type_from_str(js[key]["click"]["action"]), js[key]["click"]["data"]);
			sec_keys[i]   = std::make_pair(macro_type_from_str(js[key]["secondary"]["action"]), js[key]["secondary"]["data"]);
		} else {
			click_keys[i] = std::make_pair(MacroType::Text, "Default macro");
			sec_keys[i]   = std::make_pair(MacroType::Text, "Default macro");
		}
	}
}

auto
Config::macro_type_from_str(std::string const& str) -> MacroType
{
	if(str == "text")     return MacroType::Text;
	if(str == "sequence") return MacroType::Sequence;
	if(str == "run")      return MacroType::Run;

	throw std::runtime_error{"bad string value"};
}

auto
Config::get_config_path() -> std::filesystem::path
{
#ifdef __linux__
	return
		std::filesystem::path{std::string{getenv("HOME")}} /
		".config" /
		"macropad.json";
#else // asume it's windows
	return
		std::filesystem::path{std::string{getenv("HOMEPATH")}} /
		"macropad.json";
#endif
}
