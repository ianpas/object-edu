#pragma once

#include "object.h"

namespace Type
{
	using ObjectRef = JS::Reserved::Object::ObjectRef;

	using ObjectConstructorFunction = std::function<ObjectRef()>;
	using KeysMethod = std::function<std::vector<std::string>(ObjectRef)>;

	using FunctionConstructorFunction = std::function<ObjectRef()>;
}

namespace JS
{
	/*
		in js, you call constructor function to create new object,
		these constructor functions such as Object, Function, etc...
		are in fact instances of empty "Object", or more accurately map of string -> any

		essence: function is callable object

		to simulate the behavior of something likes "const obj = new Object()",
		with Object is actually an object bear in mind,
		we use equivalent cpp ...->Invoke<...>...
	*/

	class BuiltIn
	{
	public:
		using EmptyObject = Reserved::Object;

		static Type::ObjectRef Object;
		static Type::ObjectRef Function;

	public:
		static void Initialize();
	};
}

