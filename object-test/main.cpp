#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

auto& Object = JS::BuiltIn::Object;
auto& Function = JS::BuiltIn::Function;

namespace ObjectTest
{

	TEST_CLASS(ObjectTest)
	{
	public:
		ObjectTest()
		{
			JS::BuiltIn::Initialize();
		}

	public:
		TEST_METHOD(create)
		{
			// const obj = new Object();
			auto obj = Object->Invoke<Type::ObjectConstructorFunction>();

			// obj.value = 1;
			obj->Set("value", 1);

			// expect(obj.value).toEqual(1);
			Assert::AreEqual(1, obj->Get<int>("value"));

			// const nest = new Object();
			auto nest = Object->Invoke<Type::ObjectConstructorFunction>();

			// nest.value = 2;
			nest->Set("value", 2);

			// obj.nest = nest;
			obj->Set("nest", nest);

			// nest.value = 3;
			nest->Set("value", 3);

			// expect(obj.nest.value).toEqual(3);
			Assert::AreEqual(3, obj->Get("nest")->Get<int>("value"));

		}

		TEST_METHOD(prototype)
		{
			// expect(Object.prototype.constructor).toEqual(Object);
			Assert::AreEqual(true, Object->Get("prototype")->Get("constructor") == Object);

			// expect(Object.prototype.__proto__).toEqual(null);
			Assert::AreEqual(true, Object->Get("prototype")->Get("__proto__") == nullptr);
		}

		TEST_METHOD(__proto__)
		{
			// const obj = new Object();
			auto obj = Object->Invoke<Type::ObjectConstructorFunction>();

			// expect(obj.__proto__).toEqual(Object.prototype);
			Assert::AreEqual(true, obj->Get("__proto__") == Object->Get("prototype"));
		}

		TEST_METHOD(access)
		{
			// Object.prototype.a = 1;
			auto prototype = Object->Get("prototype");
			prototype->Set("a", 1);

			// const obj = new Object();
			auto obj = Object->Invoke<Type::ObjectConstructorFunction>();

			{
				// const obj_keys = Object.keys();
				auto obj_keys = Object->Get("keys")->Invoke<Type::KeysMethod>(obj);
				auto prototype_keys = Object->Get("keys")->Invoke<Type::KeysMethod>(prototype);
				Assert::AreEqual(true, std::find(prototype_keys.begin(), prototype_keys.end(), "a") != prototype_keys.end());
			}

			// expect(obj.a).toEqual(1);
			Assert::AreEqual(1, obj->Get<int>("a"));

			// delete Object.prototype.a;
			auto count = prototype->Delete<int>("a");

			// expect(Object.prototype.a).toEqual(undefined);
			Assert::AreEqual(true, count == 1);

			{
				auto obj_keys = Object->Get("keys")->Invoke<Type::KeysMethod>(obj);
				auto prototype_keys = Object->Get("keys")->Invoke<Type::KeysMethod>(prototype);

				// expect(Object.prototype.a).toEqual(undefined);
				Assert::AreEqual(true, std::find(prototype_keys.begin(), prototype_keys.end(), "a") == prototype_keys.end());
			}
		}

		TEST_METHOD(function)
		{
			// const f = new Function();
			auto f = Function->Invoke<Type::FunctionConstructorFunction>();

			// expect(f.__proto__).toEqual(Function.prototype);
			Assert::AreEqual(true, f->Get("__proto__") == Function->Get("prototype"));

			// expect(Function.prototype.__proto__).toEqual(Object.prototype);
			Assert::AreEqual(true, Function->Get("prototype")->Get("__proto__") == Object->Get("prototype"));

			// expect(Function.__proto__).toEqual(Function.prototype);
			Assert::AreEqual(true, Function->Get("__proto__") == Function->Get("prototype"));

			// expect(Object.__proto__).toEqual(Function.prototype);
			Assert::AreEqual(true, Object->Get("__proto__") == Function->Get("prototype"));

			// expect(Function.prototype.constructor).toEqual(Function);
			Assert::AreEqual(true, Function->Get("prototype")->Get("constructor") == Function);
		}

		TEST_METHOD(constructor_function)
		{
			/*
				function Person(age)
				{
					 this.name = "bob";
					 this.age = age;
					 this.greet = () => { return "hi" };
				}
			*/
			auto Person = Function->Invoke<Type::FunctionConstructorFunction>();

			auto prototype = Object->Invoke<Type::ObjectConstructorFunction>();
			prototype->Set("constructor", Person);

			Person->Set("prototype", prototype);

			auto code = [prototype](int age)
			{
				Type::ObjectRef __this = Object->Invoke<Type::ObjectConstructorFunction>();

				/*
					reset __proto__ to link to Person.prototype
				*/
				__this->Set("__proto__", prototype);

				/*
					the code you write in constructor function: 

					this.name = "bob";
					this.age = age;
					this.greet = () => { return "hi" };

				*/
				__this->Set("name", std::string{ "bob" });
				__this->Set("age", age);

				auto greet = Function->Invoke<Type::FunctionConstructorFunction>();
				greet->Set<std::function<std::string()>>("code", [] {return "hi"; });

				__this->Set("greet", greet);

				return __this;
			};
			using PersonConstructorFunction = std::function<Type::ObjectRef(int)>;
			Person->Set<PersonConstructorFunction>("code", code);

			// expect(Person.__proto__).toEqual(Function.prototype);
			Assert::AreEqual(true, Person->Get("__proto__") == Function->Get("prototype"));

			// expect(Person.prototype.constructor).toEqual(Person);
			Assert::AreEqual(true, Person->Get("prototype")->Get("constructor") == Person);

			// expect(Person.prototype.__proto__).toEqual(Object.prototype);
			Assert::AreEqual(true, Person->Get("prototype")->Get("__proto__") == Object->Get("prototype"));


			// const person = new Person(12);
			auto person = Person->Invoke<PersonConstructorFunction>(12);

			// expect(person.name).toEqual("bob");
			Assert::AreEqual(std::string{ "bob" }, person->Get<std::string>("name"));

			// expect(person.age).toEqual(12);
			Assert::AreEqual(12, person->Get<int>("age"));

			// expect(person.greet()).toEqual("hi");
			Assert::AreEqual(std::string{ "hi" }, person->Get("greet")->Invoke<std::function<std::string()>>());

			// expect(person.__proto__).toEqual(Person.prototype);
			Assert::AreEqual(true, person->Get("__proto__") == Person->Get("prototype"));
		}

	};
}