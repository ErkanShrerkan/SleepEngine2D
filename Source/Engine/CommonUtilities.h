#pragma once
#include <string>
#include <vector>

#define EnableFunctionIfTypeIsDerived(Base, Derived, ReturnType) typename std::enable_if<std::is_base_of<Base, Derived>::value, ReturnType>::type   
//#define EnableFunctionIfTypeIsDerivedDeclaration(Base, Derived, ReturnType) typename std::enable_if<std::is_base_of<Base, Derived>::value, ReturnType>::type* = nullptr
//#define EnableFunctionIfTypeIsDerivedDefinition(Base, Derived, ReturnType) typename std::enable_if<std::is_base_of<Base, Derived>::value, ReturnType>::type*

template <class Instance>
Instance& Singleton()
{
	static Instance instance;
	return instance;
}
