# Motivation
I'm new to js from cpp and I want to transfer my knowledge from one programming language to another. However it's hard, js brings new ideas of programming to me, such as functional programming, and of course the special object model.

As I learn js, I want to understand object and prototype, I find this [article](https://github.com/mqyqingfeng/Blog/issues/2) is helpful. An idea comes up when I finished reading, why not simulate examples in cpp?

Of course the ultimate way to understand a programming language is to implement it. However it's time consuming when your target is binary code or bytecode. As the essence of compilation is just "translation", so we can translate js code to cpp, that's another way to "implement" it.

# Scope

We will simulate [these behaviors](https://github.com/ianpas/object-edu/blob/master/object-test/main.test.js) in cpp, the approach taken is to translate it line by line.

```js
test("create", () =>
{
    const obj = new Object();
    obj.value = 1;
    expect(obj.value).toEqual(1);

    const nest = new Object();
    nest.value = 2;

    obj.nest = nest;
    nest.value = 3;

    expect(obj.nest.value).toEqual(3);
});

test("prototype", () =>
{
    expect(Object.prototype.constructor).toEqual(Object);
    expect(Object.prototype.__proto__).toEqual(null);
});

test("__proto__", () =>
{
    const obj = new Object();
    expect(obj.__proto__).toEqual(Object.prototype);
});

test("access", () =>
{

    Object.prototype.a = 1;

    const obj = new Object();
    expect(obj.a).toEqual(1);
    expect(obj.__proto__.a).toEqual(1);

    delete Object.prototype.a;
    expect(Object.prototype.a).toEqual(undefined);
});


test("function", () =>
{
    const f = new Function();
    expect(f.__proto__).toEqual(Function.prototype);

    expect(Function.prototype.__proto__).toEqual(Object.prototype);
    expect(Function.__proto__).toEqual(Function.prototype);
    expect(Object.__proto__).toEqual(Function.prototype);
    expect(Function.prototype.constructor).toEqual(Function);

});

test("constructor function", () =>
{
    function Person(age)
    {
        this.name = "bob";
        this.age = age;
        this.greet = () => "hi";
    }

    expect(Person.__proto__).toEqual(Function.prototype);
    expect(Person.prototype.constructor).toEqual(Person);
    expect(Person.prototype.__proto__).toEqual(Object.prototype);

    const person = new Person(12);
    expect(person.name).toEqual("bob");
    expect(person.age).toEqual(12);
    expect(person.greet()).toEqual("hi");

    expect(person.__proto__).toEqual(Person.prototype);
});

```

# Local setup

There are two parts of this project, cpp part and js part.

For js part, open /object-test and run "npm install" and then "npm test", or you can explore the code we want to simulate directly.

For cpp part, open JsFromCpp.sln, in this solution, there are two projects. One project named as "js" is the implementation of js object, another named as "object-test" is a cpp unit test project corresponding the one in js part.


# Examples
 
You can find all codes in this part in /object-test/main.cpp. In each example you will see cpp code commented with corresponding js one. 


## Create simple object

Test method is one to one, for example, you can also find "create" unit test in /object-test/main.test.js. Then code in js is pasted as comment for ease of reference.

```cpp
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
```

## Prototype

```cpp
TEST_METHOD(prototype)
{
    // expect(Object.prototype.constructor).toEqual(Object);
    Assert::AreEqual(true, Object->Get("prototype")->Get("constructor") == Object);

    // expect(Object.prototype.__proto__).toEqual(null);
    Assert::AreEqual(true, Object->Get("prototype")->Get("__proto__") == nullptr);
}
```
## \_\_proto__

```cpp
TEST_METHOD(__proto__)
{
    // const obj = new Object();
    auto obj = Object->Invoke<Type::ObjectConstructorFunction>();

    // expect(obj.__proto__).toEqual(Object.prototype);
    Assert::AreEqual(true, obj->Get("__proto__") == Object->Get("prototype"));
}
```

## Find property in prototype chain

```cpp
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

    // expect(obj.__proto__.a).toEqual(1);
    Assert::AreEqual(1, obj->Get("__proto__")->Get<int>("a"));

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
```

## Function and prototype

Simulate the intricate relation between function prototype and object prototype.

```cpp
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
```

## How constructor function works

```cpp
TEST_METHOD(constructor_function)
{
    /*
        function Person(age)
        {
                this.name = "bob";
                this.age = age;
                this.greet = () => "hi";
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
            this.greet = () => "hi";

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
```

# Implementation

As you see in examples, we have only two global object: Object and Function. We DIDN'T use C++ class to simulate the object model, instead we simulate it in js way - just using object instance.

In /js/object.h, we define Object class only for internal use, you can treate it as internal implementation of js. And in /js/js.cpp we create Object and Function objects. 

The difference is that object created from internal Object class is just a wrapper of key-value pairs, absolutely empty, without prototype, \_\_proto__, ... etc.

For example, the "Object" you use in js is implemented in this project as follows:

1. Create an empty object in C++
    ```cpp
    Type::ObjectRef BuiltIn::Object = std::make_shared<BuiltIn::EmptyObject>();
    ```

2. Create the prototype property
   
    ```cpp
    /*
        built-in Object prepares prototype as __proto__ for new object created with new Object()
    */
    auto prototype = std::make_shared<EmptyObject>();
    prototype->Set("constructor", BuiltIn::Object);
    prototype->Set("__proto__", std::shared_ptr<Type::ObjectRef>{nullptr});
    BuiltIn::Object->Set("prototype", prototype);
    ```

3. Create object methods, Object.keys as an example
  
    ```cpp
    /*
        methods of Object
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
    ```

4. Create the function body when you call new Object()
    ```cpp
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
    ```

As for Object class the "Object instanced" created from, there are some utility methods such as the one makes finding property in prototype chain possible:

```cpp
...
    template<typename T = ObjectRef>
    T Get(std::string key)
    {
        try
        {
            return ObjectBase::Get<T>(key);
        }
        catch (std::out_of_range const& e)
        {
            // find property in prototype chain
            return this->Get("__proto__")->Get<T>(key);
        }
    }
...
```

That's all, play with the code as you like!

# License
MIT
