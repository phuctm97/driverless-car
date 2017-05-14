#include "WindowsKeyboard.h"

void sb::WindowsKeyboard::checkBufferedKey()
{
	int key = cv::waitKey( _delay );

	if ( key > 0 ) {
		for ( auto& handler : _keyboardEventHandlers ) handler( key );
	}
}

void sb::WindowsKeyboard::addKeyboardCallback( const std::function<void( int )>& callback )
{
	_keyboardEventHandlers.push_back( callback );
}

void sb::WindowsKeyboard::release()
{
	_keyboardEventHandlers.clear();
}
