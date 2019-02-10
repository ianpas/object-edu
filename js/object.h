#pragma once
#include <memory>
#include <functional>
#include <map>
#include <string>
#include <algorithm>
#include <utility>

namespace JS
{
	namespace Reserved
	{
		class PropBase
		{
		public:

		};

		template<typename T>
		class Property : public PropBase
		{
		public:
			T m_Prop;

		public:
			Property(T prop)
			{
				m_Prop = prop;
			}

			T Get()
			{
				return m_Prop;
			}
		};


		class ObjectBase
		{
		public:
			using PlainObject = std::map<std::string, std::shared_ptr<PropBase>>;
			PlainObject m_Object;

		public:
			template<typename T>
			T Get(std::string key)
			{
				try
				{
					auto property_ptr = m_Object.at(key).get(); // PropBase<T>*
					return static_cast<Property<T>*>(property_ptr)->Get();
				}
				catch (std::out_of_range const& e)
				{
					throw std::out_of_range("property \"" + key + "\" not found!");
				}
			}

			template<typename T>
			ObjectBase& Set(std::string key, T value)
			{
				m_Object[key] = std::make_shared<Property<T>>(value);
				return *this;
			}

			template<typename T>
			PlainObject::size_type Delete(std::string key)
			{
				return m_Object.erase(key);
			}
		};

		class Object : public ObjectBase
		{
		public:
			using ObjectRef = std::shared_ptr<Object>;

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

			template<typename T, typename ...Args>
			typename T::result_type Invoke(Args&&... args)
			{
				try
				{
					return this->Get<T>("code")(std::forward<Args>(args)...);
				}
				catch (std::out_of_range const& e)
				{
					throw std::out_of_range("this object is not callable!");
				}
			}


		};
	}

}
