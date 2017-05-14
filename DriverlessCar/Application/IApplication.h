#ifndef __SB_APPLICATION_H__
#define __SB_APPLICATION_H__
#include "Keyboard/IKeyboard.h"

namespace sb
{
class IApplication
{
public:
	virtual ~IApplication() = default;

	virtual void run() = 0;

	virtual void exit() = 0;

	virtual void release() = 0;

	virtual void addKeyboardCallback( const std::function<void( int )>& callback ) = 0;
};
}

#endif //!__SB_APPLICATION_H__
