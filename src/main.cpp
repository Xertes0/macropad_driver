#include <unistd.h>
#include <fcntl.h>

#include "xdo.hpp"
#include "config.hpp"

constexpr auto PATH{"/dev/ttyACM0"};

auto
main() -> int
{
	Config config{};
	XDO    xdo{};

	int fd = open(PATH, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "Failed to open\n");
		return 1;
	}

	while(true) {
		char buffer[KEY_COUNT+1] = {'\0'};
		auto const bytes_read = read(fd, buffer, sizeof(buffer));
		if(bytes_read != 0 && buffer[0] >= '0' && buffer[0] <= '9') {
			printf("%s\n", buffer);
			for(size_t i=0;i<KEY_COUNT-1;i++) {
				if(buffer[i] == '1') {
					auto const& [type, data] = config.keys[i];
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
