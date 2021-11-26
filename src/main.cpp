#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <xdo.h>

#include <array>
#include <utility>
#include <string_view>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#include <nlohmann/json.hpp>

constexpr auto PATH{"/dev/ttyACM0"};

class XDO
{
public:
	XDO() :
		m_xdo{xdo_new(nullptr)}
	{

	};

	void
	send_text(char const * const text)
	{
		xdo_enter_text_window(m_xdo, CURRENTWINDOW, text, DELAY);
	}

	void
	send_key(char const * const key)
	{
		xdo_send_keysequence_window(m_xdo, CURRENTWINDOW, key, DELAY);
	}

private:
	static constexpr useconds_t DELAY{25000};
	xdo_t *m_xdo;
};

enum class MacroType
{
	Text,
	Sequence,
};

constexpr size_t KEY_COUNT{10};
constexpr auto CONF_PATH{"/home/user/.config/macropad.json"};

struct Config
{
public:
	Config() :
		keys{}
	{
		std::ifstream file{CONF_PATH};
		std::stringstream sstream{};
		sstream << file.rdbuf();

		auto const js = nlohmann::json::parse(sstream.str());
		for(size_t i=0;i<KEY_COUNT;i++) {
			auto const key = std::string{static_cast<char>('0' + i)};
			if(js.contains(key)) {
				std::cout << i << ':' << js[key] << '\n';
				keys[i] = std::make_pair(macro_type_from_str(js[key]["click"]["action"]), js[key]["click"]["data"]);
			} else {
				keys[i] = std::make_pair(MacroType::Text, "Default macro");
			}
		}
	};

	std::array<std::pair<MacroType, std::string>, KEY_COUNT> keys;

private:
	static auto
	macro_type_from_str(std::string const &str) -> MacroType
	{
		if(str == "text")     return MacroType::Text;
		if(str == "sequence") return MacroType::Sequence;

		throw new std::runtime_error{"bad string value"};
	}
};

int main()
{
	Config config;
	XDO xdo;
	//xdo.send_text("adfab");
	//xdo.send_key("XF86AudioLowerVolume+XF86AudioMute");
	int fd = open(PATH, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "Failed to open\n");
		return 1;
	}

	while(true) {
		char buffer[KEYS.size()+1] = {'\0'};
		auto const bytes_read = read(fd, buffer, sizeof(buffer));
		if(bytes_read != 0 && buffer[0] >= '0' && buffer[0] <= '9') {
			printf("%s\n", buffer);
			for(size_t i=0;i<KEYS.size();i++) {
				if(buffer[i] == '1') {
					auto const &[type, data] = config.keys[i];
					switch(type) {
						case MacroType::Text:     xdo.send_text(data.data()); break;
						case MacroType::Sequence: xdo.send_key(data.data());  break;
					}
				}
			}
		}
		usleep(20);
	}

	return 0;
}
