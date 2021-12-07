#ifndef MACROPAD_DRIVER_CONFIG_HPP
#define MACROPAD_DRIVER_CONFIG_HPP

#include <array>
#include <string>
#include <filesystem>

constexpr size_t KEY_COUNT{10};

enum class MacroType
{
	Text,
	Sequence,
	Run,
};

struct Config
{
public:
	Config();

	std::array<std::pair<MacroType, std::string>, KEY_COUNT> click_keys;
	std::array<std::pair<MacroType, std::string>, KEY_COUNT> sec_keys;

	void
	reload();

private:
	static auto
	macro_type_from_str(std::string const& str) -> MacroType;

	static auto
	get_config_path() -> std::filesystem::path;
};

#endif // MACROPAD_DRIVER_CONFIG_HPP
