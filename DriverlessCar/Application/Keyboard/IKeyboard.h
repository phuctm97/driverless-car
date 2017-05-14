#ifndef __SB_I_KEYBOARD_H__
#define __SB_I_KEYBOARD_H__
#include <functional>

namespace sb
{
class IKeyboard
{
public:
	virtual ~IKeyboard() = default;

	virtual void checkBufferedKey() = 0;

	virtual void addKeyboardCallback( const std::function<void( int )>& callback ) = 0;

	virtual void release() = 0;
};
}

#endif
