#include "js.h"
#include <iostream>

namespace JS
{

	Type::ObjectRef BuiltIn::Object = std::make_shared<BuiltIn::EmptyObject>();
	Type::ObjectRef BuiltIn::Function = std::make_shared<BuiltIn::EmptyObject>();

	void BuiltIn::Initialize()
	{
		/*
			Object
		*/
		{
			/*
				built-in Object prepares prototype as __proto__ for new object created with new Object()
			*/
			auto prototype = std::make_shared<EmptyObject>();
			prototype->Set("constructor", BuiltIn::Object);
			prototype->Set("__proto__", std::shared_ptr<Type::ObjectRef>{nullptr});
			BuiltIn::Object->Set("prototype", prototype);

			/*
				method of Object
			*/
			auto keys = std::make_shared<EmptyObject>();
			keys->Set<Type::KeysMethod>("code", [](Type::ObjectRef obj) {
				std::vector<std::string> __keys;
				std::transform(
					obj->m_Object.begin(),
					obj->m_Object.end(),
					std::back_inserter(__keys),
					[](decltype(obj->m_Object)::value_type const& pair)
				{
					return pair.first;
				});
				return __keys;
			});

			BuiltIn::Object->Set("keys", keys);

			/*

			*/
			auto code = [prototype]
			{
				auto __this = std::make_shared<EmptyObject>();
				/*
					use __proto__ to link to parent's prototype
				*/
				__this->Set("__proto__", prototype);
				return __this;
			};
			BuiltIn::Object->Set<Type::ObjectConstructorFunction>("code", code);
		}

		/*
			Function
		*/
		{
			/*
				prepares prototype as __proto__ for new object created with new Function()
			*/
			auto prototype = std::make_shared<EmptyObject>();
			prototype->Set("constructor", BuiltIn::Function);
			prototype->Set("__proto__", BuiltIn::Object->Get("prototype"));
			BuiltIn::Function->Set("prototype", prototype);
			BuiltIn::Function->Set("__proto__", prototype);
			BuiltIn::Object->Set("__proto__", prototype);

			/*

			*/
			auto code = [prototype]
			{
				auto __this = std::make_shared<EmptyObject>();
				/*
					use __proto__ to link to parent's prototype
				*/
				__this->Set("__proto__", prototype);
				return __this;
			};
			BuiltIn::Function->Set<Type::ObjectConstructorFunction>("code", code);
		}
	}
}