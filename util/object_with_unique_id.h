#ifndef OBJECT_WITH_UNIQUE_ID_H_INCLUDED
#define OBJECT_WITH_UNIQUE_ID_H_INCLUDED

#include <unordered_map>
#include <type_traits>
#include <stdexcept>

namespace atmik
{
	namespace util
	{
		// KEY_TYPE must be LessThanComparable
		// the first arg to the constructor of derived must be the unique key
		// this is the base class used by components (sim,group etc.) with unique id
		template < typename DERIVED, typename KEY_TYPE = unsigned long long > 
		struct object_with_unique_id
		{

			const KEY_TYPE& id() const noexcept { return id_; }

			object_with_unique_id(const KEY_TYPE& id) : id_(id) {}
			~object_with_unique_id() = default;

			// non-copyable, non-moveable, non-assignable
			object_with_unique_id(const object_with_unique_id& that) = delete;
			object_with_unique_id(object_with_unique_id&& that) = delete;

			template < typename... OTHER_CTOR_ARGS >
			static DERIVED& try_create( const KEY_TYPE& key, OTHER_CTOR_ARGS&&... args)
			{
				return objects().try_emplace( key, key, std::forward<OTHER_CTOR_ARGS>(args)...).first->second;
			}

			template < typename... OTHER_CTOR_ARGS >
			static DERIVED& create(const KEY_TYPE& key, OTHER_CTOR_ARGS&&... args)
			{
				if (exists(key)) throw std::domain_error( "duplicate id" );
				return try_create(key, std::forward<OTHER_CTOR_ARGS>(args)...);
			}

			static DERIVED& look_up( const KEY_TYPE& id) // will throw if the id is invalid
			{
				return objects().at(id);
			}

			static void erase(const KEY_TYPE& id) { objects().erase(id); }

			static bool exists(const KEY_TYPE& id)
			{
				return objects().find(id) != objects().end();
			}

			const KEY_TYPE id_ ;

			static std::unordered_map<KEY_TYPE, DERIVED>& objects();
		};


		template < typename DERIVED, typename KEY_TYPE >
		std::unordered_map<KEY_TYPE, DERIVED>& object_with_unique_id<DERIVED,KEY_TYPE>::objects()
		{
			static std::unordered_map<KEY_TYPE, DERIVED> map;
			return map;
		}
	}
}

#endif // OBJECT_WITH_UNIQUE_ID_H_INCLUDED

