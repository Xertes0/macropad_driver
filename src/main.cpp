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
	static constexpr useconds_t DELAY{50000};
	xdo_t *m_xdo;
};

enum class MacroType
{
	Text,
	Sequence,
};

constexpr size_t KEY_COUNT{10};
constexpr std::array<std::pair<MacroType, std::string_view>, KEY_COUNT> KEYS {
	std::make_pair(MacroType::Sequence, "Up"),
	std::make_pair(MacroType::Text,     "adfa"),
	std::make_pair(MacroType::Sequence, "C"),

	std::make_pair(MacroType::Text,     "adfa"),
	std::make_pair(MacroType::Sequence, "Right"),
	std::make_pair(MacroType::Text,     "adfa"),

	std::make_pair(MacroType::Sequence, "Down"),
	std::make_pair(MacroType::Text,     "adfa"),
	std::make_pair(MacroType::Sequence, "Left"),

	std::make_pair(MacroType::Text,     "adfa"),
};

constexpr auto CONF_PATH{"/home/piotr/.config/macropad.json"};

auto
get_conf()
{
	std::ifstream file{CONF_PATH};
	std::stringstream sstream{};
	sstream << file.rdbuf();

	auto const js = nlohmann::json::parse(sstream.str());
	for(size_t i=0;i<KEY_COUNT;i++) {
		auto const key = std::string{static_cast<char>('0' + i)};
		if(js.contains(key)) {
			std::cout << i << ':' << js[key] << '\n';
		}
	}
}

int main()
{
	get_conf();
	return 0;
	XDO xdo;
	//xdo.send_text("adfab");
	//xdo.send_key("XF86AudioLowerVolume+XF86AudioMute");
	int fd = open(PATH, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "Failed to open\n");
		return 1;
	}

	while(true) {
		char buffer[4] {'\0'};
		auto const bytes_read = read(fd, buffer, sizeof(buffer));
		if(bytes_read != 0 && buffer[0] >= '0' && buffer[0] <= '9') {
			printf("%s\n", buffer);
			auto const &[type, data] = KEYS[buffer[0] - '0'];
			switch(type) {
				case MacroType::Text:     xdo.send_text(data.data()); break;
				case MacroType::Sequence: xdo.send_key(data.data());  break;
			}
		}
		usleep(20);
	}

	return 0;
}
