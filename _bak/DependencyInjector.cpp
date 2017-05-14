#include "DependencyInjector.h"

sb::DependencyInjector* sb::DependencyInjector::_instance = nullptr;

template<class TBase, class TDerive>
int sb::DependencyInjector::registerType()
{
	return 0;
}

template<class T>
void sb::DependencyInjector::registerObject() { }

template<class TDependent>
void sb::DependencyInjector::acknowledgeDependent( const std::string& typeName, const std::vector<std::string>& dependentTypeNames ) { }

template<class TPrincipal>
void sb::DependencyInjector::acknowledgePrincipal( const std::string& typeName, const std::vector<std::string>& baseTypes ) { }

template<class T>
int sb::DependencyInjector::resolve( T* p )
{
	// if T is an abstract class or interface, look at the registered information to find resolving type, consider that type as T and continue

	// go through the collection of T's constructors ordered by number of parameter

	// if it has a default constructor -> use it to construct -> finish

	// do again with each parameter in the constructor

	// if all parameters in a constructor resolved -> use it to construct -> finish

	// continue

	// can't resolve any constructor -> throw exception

	return 0;
}

sb::DependencyInjector* sb::DependencyInjector::getInstance()
{
	if ( _instance == nullptr ) {
		_instance = new DependencyInjector();
	}

	return _instance;
}

void sb::DependencyInjector::releaseInstance()
{
	if ( _instance != nullptr ) {
		delete _instance;
		_instance = nullptr;
	}
}
