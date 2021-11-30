#ifndef MACROPAD_DRIVER_XDO_HPP
#define MACROPAD_DRIVER_XDO_HPP

#include <xdo.h>

class XDO
{
public:
	XDO() :
		m_xdo{xdo_new(nullptr)} {}

	void
	send_text(char const *const text)
	{
		xdo_enter_text_window(m_xdo, CURRENTWINDOW, text, DELAY);
	}

	void
	send_key(char const *const key)
	{
		xdo_send_keysequence_window(m_xdo, CURRENTWINDOW, key, DELAY);
	}

private:
	static constexpr useconds_t DELAY{20000};
	xdo_t* m_xdo;
};

#endif // MACROPAD_DRIVER_XDO_HPP
