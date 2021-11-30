#ifndef MACROPAD_DRIVER_CONFIG_HPP
#define MACROPAD_DRIVER_CONFIG_HPP

#include <array>
#include <string>

constexpr size_t KEY_COUNT{10};
constexpr auto CONF_PATH{"/home/user/.config/macropad.json"};

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
};

#endif // MACROPAD_DRIVER_CONFIG_HPP
