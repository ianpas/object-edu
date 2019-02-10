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
    // TODO: anonymous constructor of f.prototype
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

// TODO: two types of properties: data property and accessor property