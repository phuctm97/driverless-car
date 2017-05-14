#ifndef __SB_WINDOWS_KEYBOARD_H__
#define __SB_WINDOWS_KEYBOARD_H__

#include <opencv2/opencv.hpp>
#include "IKeyboard.h"
#include <vector>

namespace sb
{
class WindowsKeyboard : public IKeyboard
{
private:
	int _delay;

	std::vector<std::function<void( int )>> _keyboardEventHandlers;

public:
	WindowsKeyboard( int delay )
		: _delay( delay ) {}

	void checkBufferedKey() override;

	void addKeyboardCallback( const std::function<void( int )>& callback ) override;

	void release() override;
};
}

#endif //!__SB_WINDOWN_KEYBOARD_H__
