#ifndef MACROPAD_DRIVER_XDO_HPP
#define MACROPAD_DRIVER_XDO_HPP

#include <algorithm>
#include <string>

#include <unistd.h>
#include <xdo.h>

enum class KeyState
{
	Down,
	Up,
};

class XDO
{
public:
	XDO() :
		m_xdo{xdo_new(nullptr)} {}

	void
	send_text(std::string const& text) const
	{
		xdo_enter_text_window(m_xdo, CURRENTWINDOW, text.c_str(), DELAY);
	}

	void
	send_comb(std::string const& keys) const
	{
		send_keys_state<KeyState::Down>(keys);
		usleep(DELAY);
		send_keys_state<KeyState::Up>(keys);
	}

private:
	static constexpr useconds_t DELAY{12000};
	xdo_t* m_xdo;

	template<KeyState STATE>
	void
	send_keys_state(std::string const& keys) const
	{
		using enum KeyState;
		auto begin_iter = [](auto const& keys){
			if constexpr(STATE == Down)
				return std::begin(keys);
			else
				return std::rbegin(keys);
		};
		auto end_iter = [](auto const& keys){
			if constexpr(STATE == Down)
				return std::end(keys);
			else
				return std::rend(keys);
		};

		auto substr_values = [&](auto last, auto found){
			if constexpr(STATE == Down) {
				return std::make_tuple(
					last - begin_iter(keys),
					found - begin_iter(keys)
				);
			} else {
				return std::make_tuple(
					keys.length() - (found - begin_iter(keys)),
					keys.length() - (last - begin_iter(keys))
				);
			}
		};

		auto last = begin_iter(keys);
		auto end = false;
		while(!end) {
			auto found = std::find(last, end_iter(keys), '+');
			if(found == end_iter(keys)) {
				end = true;
			}

			auto const [first, second] = substr_values(last, found);
			auto key = keys.substr(first, second);

			if constexpr(STATE == Down)
				xdo_send_keysequence_window_down(m_xdo, CURRENTWINDOW, key.c_str(), DELAY);
			else
				xdo_send_keysequence_window_up(m_xdo, CURRENTWINDOW, key.c_str(), DELAY);

			last = found+1;
		}
	}
};

#endif // MACROPAD_DRIVER_XDO_HPP
