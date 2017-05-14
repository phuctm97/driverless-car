#ifndef __DEPENDENCY_INJECTOR_H__
#define __DEPENDENCY_INJECTOR_H__

#include <iostream>
#include <string>
#include <vector>
#include <stack>
#include <map>

namespace sb
{
class DependencyInjector
{
private:
	static DependencyInjector* _instance;

public:
	static DependencyInjector* getInstance();

	static void releaseInstance();

private:
	std::map<std::string, std::string> _registeredTypes;

public:
	template<class TBase, class TDerive>
	int registerType();

	template<class T>
	void registerObject();

	template<class TDependent>
	void acknowledgeDependent( const std::string& typeName,
	                           const std::vector<std::string>& dependentTypeNames );

	template<class TPrincipal>
	void acknowledgePrincipal( const std::string& typeName,
	                           const std::vector<std::string>& baseTypes );

	template<class T>
	int resolve( T* p );
};
}

#endif //!__DEPENDENCY_INJECTOR_H__
