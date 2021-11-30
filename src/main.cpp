#include <unistd.h>
#include <fcntl.h>

#include <algorithm>
#include <cstdio>
#include <csignal>
#include <memory>

#include "xdo.hpp"
#include "config.hpp"

constexpr auto PATH{"/dev/ttyACM0"};

constexpr uint32_t HOLD_TIMEOUT{3};
constexpr uint32_t HOLD_TIME{5};

std::unique_ptr<Config> g_config;

void
handle_signal(int signum)
{
	printf("conf reload\n");
	g_config->reload();
}

void
execute(XDO& xdo, MacroType type, char const *const data)
{
	switch(type) {
		case MacroType::Text:     xdo.send_text(data); break;
		case MacroType::Sequence: xdo.send_key(data);  break;
		case MacroType::Run:      std::system(data);   break;
	}
}

auto
main() -> int
{
	g_config = std::make_unique<Config>();
	XDO    xdo{};

	signal(SIGRTMIN+10, handle_signal);

	int fd = open(PATH, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "Failed to open\n");
		return 1;
	}
	printf("Ok\n");

	std::array<uint32_t, KEY_COUNT> last_status;
	std::fill(std::begin(last_status), std::end(last_status), 0);

	while(true) {
		char buffer[KEY_COUNT+1] = {'\0'};
		auto const bytes_read = read(fd, buffer, sizeof(buffer));
		if(bytes_read != 0 && buffer[0] >= '0' && buffer[0] <= '9') {
			//printf("%s\n", buffer);
			for(size_t i=0;i<KEY_COUNT;i++) {
				if(buffer[i] == '1') {
					if(last_status[i] != 0) {
						if(last_status[i] == HOLD_TIME) {
							//printf("hold time\n");
							auto const& [type, data] = g_config->sec_keys[i];
							//printf("data: %s\n", data.data());
							execute(xdo, type, data.data());
						}
						last_status[i]++;
						continue;
					} else {
						last_status[i] = 1;
					}
				} else {
					if(last_status[i] != 0 && last_status[i] <= HOLD_TIMEOUT) {
						auto const& [type, data] = g_config->click_keys[i];
						//printf("data: %s\n", data.data());
						execute(xdo, type, data.data());
					}
					last_status[i] = 0;
				}
			}
		}
		usleep(10);
	}

	return 0;
}
