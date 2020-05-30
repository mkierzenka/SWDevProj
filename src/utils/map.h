#pragma once

#include "object.h"
#include "string.h"
#include "num.h"
#include <assert.h>
#include <unordered_map>


struct myMapHash {
	size_t operator()(Object* a) const {
		return a->hash();
	}
};

struct myMapEquals {
	bool operator()(Object* a, Object* b) const {
		return a->equals(b);
	}
};


/**
 * Represents a map where elements are mapped from key to value. A map can return its value by key
 * at O(1) time because each key is unique and grabbed by index, there are no duplicate keys.
 * There may be duplicate values, though.
 * 
 * NOTE: This parent Map class will deal strictly with key-value pairs of (Object, Object). 
 * IT TAKES OWNERSHIP OF KEYS AND VALUES!
 * 
 * If you wish to type-check specifically, use the SOMap and SSMap accordingly. If you wish to add 
 * more specific maps that you think will be useful, make a pull request here: 
 * https://github.com/csstransky/cs4500-assignment-1-part-2
 */
class Map : public Object
{
public:
	std::unordered_map<Object*, Object*, myMapHash, myMapEquals> inner_;

	/**
	 * @brief Construct a new Map object
	 * 
	 */
	Map() : Map(4) {}

	/**
	 * Constructor to optimize performance of map. !!Argument is ignored!!
	 * @param capacity number of buckets available to store different values.
	 */
	Map(size_t capacity) : Object() {}

	/**
	 * @brief Destroy the Map object
	 * 
	 */
	~Map()
	{
		deleteData_();
		inner_.clear();
	}

	// Helper method for equals()
	// Return true if an object in in a CwC array of objects of given size, else return false
	bool isInArr_(Object *needle, Object **haystack, size_t haystackLen)
	{
		for (size_t i = 0; i < haystackLen; i++)
		{
			if (haystack[i]->equals(needle))
			{
				return true;
			}
		}
		return false;
	}

	/**
	 * @brief Determines if this map equals the given object. For two maps to be equal, they must
	 * contain the same key-value pairs, BUT do not need to share the same locations inside
	 * of the buckets.
	 * 
	 * @param other object to compare to
	 * @return equals or not
	 */
	virtual bool equals(Object *other)
	{
		if (this == other)
		{
			return true;
		}
		Map *otherMap = dynamic_cast<Map *>(other);
		if (!otherMap || this->size() != otherMap->size())
		{
			return false;
		}
		for (const std::pair<Object*, Object*> kvpair : inner_)
		{
			Object* curKey = kvpair.first;
			Object* curValue = kvpair.second;
			if (!(otherMap->contains_key(curKey)) || !(curValue->equals(otherMap->get(curKey))))
			{
				return false;
			}
		}
		// above is sufficient for equality b/c:
		//   same # of keys, no duplicate keys within a map => keys in this and otherMap are same
		//   checks that this map's value for given key equals otherMap's value for that key
		return true;
	}

	/**
	 * @brief Calculates the hash of the map. The hash will only be dependent on the key value pairs
	 * of the map, and NOT the location of them in the buckets.
	 * Example:
	 * Map* map1 = new Map(10000);
	 * Map* map2 = new Map();
	 * Object* key = new Object();
	 * Object* value = new Object();
	 * map1->put(key, value);
	 * map2->put(key, value);
	 * // This should return true even though the key value pairs are in different bucket locations
	 * map1->hash() == map2->hash(); 
	 * 
	 * @return hash representation of each key value pair in the map
	 */
	virtual size_t hash()
	{
		size_t hash = 0;
		for (const std::pair<Object* const, Object*> kvpair : inner_) {
			hash += kvpair.first->hash();
			hash += kvpair.second->hash();
		}
		return hash;
	}

	/**
	 * @brief Determines the number of mappings in this map.
	 * @return size of map
	 */
	virtual size_t size()
	{
		return inner_.size();
	}

	/**
	 * Return capacity of the map
	 */
	size_t capacity()
	{
		return inner_.max_size();
	}

	/**
	 * Adds a mapping from key to val to the map. If there is already a key
	 * of the same value in the map, the value will be overwritten by the new value.
	 * Caller is responsible for deleting returned value.
	 * If the key already exists in the map, caller responsible for deleting key.
	 * 
	 * NOTE: This function will mutate the Map
	 * 
	 * @param key Object key
	 * @param val Object value
	 * @return the old value from the given key, a nullptr is returned if no old value exists
	 */
	Object *put(Object *key, Object *val)
	{
		if (!key || !val)
		{
			return nullptr;
		}
		Object* prev = nullptr;
		if (contains_key(key)) {
			prev = inner_.at(key);
			inner_[key] = val;
			delete key;
		} else {
			inner_[key] = val;
		}
		return prev;
	}

	/**
	 * Returns the actual object that the key maps to.
	 * 
	 * @param key Object unique key
	 * @return Object value for the key, a nullptr is returned if no value exists
	 */
	Object *get(Object *key)
	{
		if (!key || !contains_key(key))
		{
			return nullptr;
		}
		return inner_.at(key);
	}

	/**
	 * Removes the key value mapping from the map.
	 * Caller responsible for deleting returned value.
	 * 
	 * @param key Object key of mapping to remove
	 * @return value that was removed, a nullptr is returned if no value exists
	 */
	Object *remove(Object *key)
	{
		if (!key)
		{
			return nullptr;
		}
		auto itr = inner_.find(key);
		if (itr != inner_.end())
		{
			// found it - delete it
			Object* prev = itr->second;
			delete itr->first;
			inner_.erase(itr);
			return prev;
		} else {
			return nullptr;
		}
	}

	/**
	 * Determines if the map contains the given key.
	 * @param key Object key
	 * @return if the key is in the map
	 */
	bool contains_key(Object *key)
	{
		if (!key)
		{
			return false;
		}
		return inner_.count(key) >= 1;
	}

	/**
	 * Remove all values from the map
	 */
	void clear()
	{
		deleteData_();
		inner_.clear();
	}

	/**
	 * Removes all bucket data from the map
	 */
	void deleteData_()
	{
		for (std::pair<Object*, Object*> kvpair : inner_) {
			delete kvpair.first;
			delete kvpair.second;
		}
	}

	/**
	 * Returns a new Object array of keys. This array will need to be freed in memory after use.
	 * NOTE: If a map has no keys, it will return an empty Object**
	 * @return new Object array of keys
	 */
	virtual Object **get_keys()
	{
		Object **keys = new Object *[this->size()];
		size_t i = 0;
		for (std::pair<Object* const, Object*> kvpair : inner_) {
			Object* curKey = kvpair.first;
			keys[i] = curKey;
			i++;
		}
		return keys;
	}

	/**
	 * Returns a new Object array of values. This array will need to be freed in memory after use.
	 * NOTE: If a map has no values, it will return an empty Object**
	 * @return new Object array of values
	 */
	virtual Object **get_values()
	{
		Object **vals = new Object *[this->size()];
		size_t i = 0;
		for (std::pair<Object* const, Object*> kvpair : inner_) {
			Object* curVal = kvpair.second;
			vals[i] = curVal;
			i++;
		}
		return vals;
	}
};

/** 
 * Represents a map where the key is a string and the value is a object. Inherits from map.
 * Example: { "string_list: new StrList("hi", "bye"), "string": new String("hello") }
 * 
 * These functions are restricted to only allowing String->Object pairs, meaning that a MapStrObj
 * will only allow String->Object pairs.
 */
class MapStrObj : public Map
{
public:
	Map *helper_;

	/**
	 * Constructor
	 */
	MapStrObj()
	{
		this->helper_ = new Map();
	}

	/**
	 * Constructor to tailor performance of map.
	 * @param capacity number of buckets available to store different values.
	 */
	MapStrObj(size_t capacity)
	{
		this->helper_ = new Map();
	}

	/**
	 * Deconstructor
	 */
	~MapStrObj()
	{
		delete this->helper_;
	}

	/**
	 * Adds a mapping from key to val to the map. A key must be unique, if there is already a key
	 * of the same value in the map, the value will be overwritten by the new value. Map takes 
	 * ownership of this key and value
	 * 
	 * NOTE: This function will mutate the Map
	 * 
	 * @param key String key
	 * @param val Object value
	 * @return the old value from the given key, a nullptr is returned if no old value exists
	 */
	Object *put(String *key, Object *val)
	{
		Object *prev = this->helper_->put(key, val);
		return prev;
	}

	/**
	 * Returns the object that the key maps to.
	 * 
	 * @param key String unique key
	 * @return Object value for the key, a nullptr is returned if no value exists
	 */
	Object *get(String *key)
	{
		return this->helper_->get(key);
	}

	/**
	 * Removes the key value mapping from the map.
	 * 
	 * @param key String key of mapping to remove
	 * @return Object value that was removed, a nullptr is returned if no value exists
	 */
	Object *remove(String *key)
	{
		Object *removed = this->helper_->remove(key);
		return removed;
	}

	/**
	 * Determines if the map contains the given key.
	 * @param key String key
	 * @return if the key is in the map
	 */
	bool contains_key(String *key)
	{
		return this->helper_->contains_key(key);
	}

	/**
	 * Returns a new Object array of keys. This array will need to be freed in memory after use.
	 * NOTE: If a map has no keys, it will return an empty Object**
	 * @return new Object array of keys
	 */
	// Overrides Map.get_keys()
	Object **get_keys() override
	{
		return this->helper_->get_keys();
	}

	/**
	 * Returns a new Object array of values. This array will need to be freed in memory after use.
	 * NOTE: If a map has no values, it will return an empty Object**
	 * @return new Object array of values
	 */
	// Overrides Map.get_values()
	Object **get_values() override
	{
		return this->helper_->get_values();
	}

	/**
	 * @brief Determines the number of mappings in this map.
	 * @return size of map
	 */
	// Overrides Map.size()
	size_t size() override
	{
		return this->helper_->size();
	}

	/**
	 * @brief Determines if this map equals the given object. For two maps to be equal, they must
	 * contain the exact same key-value pairs, BUT do not need to share the same locations inside
	 * of the buckets.
	 * 
	 * @param other object to compare to
	 * @return equals or not
	 */
	// Overrides Map.equals
	bool equals(Object *other) override
	{
		if (this == other)
			return true;
		MapStrObj *otherMap = dynamic_cast<MapStrObj *>(other);
		if (!otherMap)
		{
			return false;
		}
		if (this->size() != otherMap->size())
			return false;
		// different values for capacity_ (the number of buckets) is okay
		return this->helper_->equals(otherMap->helper_);
	}

	/**
	 * @brief Calculates the hash of the map. The hash will only be dependent on the key value pairs
	 * of the map, and NOT the location of them in the buckets.
	 * Example:
	 * Map* map1 = new Map(10000);
	 * Map* map2 = new Map();
	 * Object* key = new Object();
	 * Object* value = new Object();
	 * map1->put(key, value);
	 * map2->put(key, value);
	 * // This should return true even though the key value pairs are in different bucket locations
	 * map1->hash() == map2->hash(); 
	 * 
	 * @return hash representation of each key value pair in the map
	 */
	// Overrides Map.hash
	size_t hash() override
	{
		return this->helper_->hash();
	}
};

/** Provided with Milestone 4 */
class SIMap : public Map
{
public:
	SIMap() : Map() {}
	SIMap(SIMap &other) : Map()
	{
		Object **keys = other.get_keys();	//do not delete, this is what's going into map
		Object **vals = other.get_values(); //do not delete, this is what's going into map
		size_t lenOther = other.size();
		for (size_t i = 0; i < lenOther; i++)
		{
			String *k = (dynamic_cast<String *>(keys[i]))->clone();
			Num *n = (dynamic_cast<Num *>(vals[i]))->clone();
			set(*k, n);
		}
	}
	~SIMap() {}
	Num *get(String &key) { return dynamic_cast<Num *>(Map::get(&key)); }
	Num *get(String* key) { return dynamic_cast<Num *>(Map::get(key)); }
	void set(String &k, Num *v)
	{
		assert(v);
		Map::put(&k, v);
	}
	String** get_str_keys() {
		String **keys = new String *[this->size()];
		auto myElems = inner_.begin();
		for (size_t i = 0; i < this->size(); i++)
		{
			Object* curKey = (*(myElems++)).first;
			keys[i] = (String*)curKey;
		}
		return keys;
	}
};
