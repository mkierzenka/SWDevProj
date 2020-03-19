#pragma once

#include "object.h"
#include "string.h"
#include "helper.h"
#include <cstdio>

// some inspiration from https://dzone.com/articles/custom-hashmap-implementation-in-java


/**
* The Entry class represents an Entry in a HashMap as implemented below.
* Each Entry has a Key, Value pair of Object* and a field to link it to
*   the next Entry. This Entry object is intended to be used as a linked
*   list. It takes ownership of keys and values.
*
*/
class Entry : public Object {
	public:
		Object* key_;    //owned
		Object* value_;  //owned
		Entry* next_;    //owned
		
		Entry() : Object() {
			this->key_ = nullptr;
			this->value_ = nullptr;
			this->next_ = nullptr;
		}
		
		Entry(Object* key, Object* value) : Object() {
			this->key_ = key;
			this->value_ = value;
			this->next_ = nullptr;
		}
		
		~Entry() { 
			if (key_ != NULL && key_ != nullptr) {
				delete key_;
			}
			if (value_ != NULL && value_ != nullptr) {
				delete value_;
			}
			if (next_ != NULL && next_ != nullptr) {
				delete next_;
			}
		}
		
		// Gets key field
		Object* getKey() {
			return this->key_;
		}
		
		// Sets key field
		void setKey(Object* newKey) {
			this->key_ = newKey;
		}
		
		// Gets value field
		Object* getValue() {
			return this->value_;
		}
		
		// Sets next field to newValue
		void setValue(Object* newValue) {
			this->value_ = newValue;
		}

		// Gets 'next' field
		Entry* getNext() {
			return this->next_;
		}
		
		// Sets 'next' field to newNext
		void setNext(Entry* newNext) {
			this->next_ = newNext;
		}

		// A helper method that will add keys to keysArr starting at the specified index
		// This method will recursively call itself until all Entry objects linked to this are sequentially
		//   added to keysArr. ASSUME: keysArr is big enough to hold all entries
		int addKeysToArr(Object** keysArr, int nextIdx) {
			if (!keysArr) {
				return -1;
			}
			keysArr[nextIdx] = this->key_;
			nextIdx++;
			if (!next_ || next_ == nullptr) {
				return nextIdx;
			}
			return next_->addKeysToArr(keysArr, nextIdx);
		}

		// A helper method that will add values to valuesArr starting at the specified index
		// This method will recursively call itself until all Entry objects linked to this are sequentially
		//   added to valuesArr. ASSUME: valuesArr is big enough to hold all entries
		int addValuesToArr(Object** valuesArr, int nextIdx) {
			if (!valuesArr) {
				return -1;
			}
			valuesArr[nextIdx] = this->value_;
			nextIdx++;
			if (!next_ || next_ == nullptr) {
				return nextIdx;
			}
			return next_->addValuesToArr(valuesArr, nextIdx);
		}
};

/**
 * Represents a map where elements are mapped from key to value. A map can return its value by key
 * at O(1) time because each key is unique and grabbed by index, there are no duplicate keys.
 * 
 * NOTE: This parent Map class will deal strictly with key-value pairs of (Object, Object). 
 * IT TAKES OWNERSHIP OF KEYS AND VALUES!
 * 
 * If you wish to type-check specifically, use the SOMap and SSMap accordingly. If you wish to add 
 * more specific maps that you think will be useful, make a pull request here: 
 * https://github.com/csstransky/cs4500-assignment-1-part-2
 */
class Map : public Object {
    public:
		Entry** buckets_;
		size_t capacity_;
		size_t size_;
		
		/**
		 * @brief Construct a new Map object
		 * 
		 */
		Map() {
			capacity_ = 4;
			size_ = 0;
			buckets_ = new Entry*[capacity_];
			memset(buckets_, 0, capacity_ * sizeof(buckets_));
		}
		
		/**
		 * Constructor to optimize performance of map.
		 * @param capacity number of buckets available to store different values.
		 */
		Map(size_t capacity) {
			capacity_ = capacity;
			size_ = 0;
			buckets_ = new Entry*[capacity_];
			memset(buckets_, 0, capacity_ * sizeof(buckets_));
		}

		/**
		 * @brief Destroy the Map object
		 * 
		 */
		~Map() {
			for (size_t i = 0; i < capacity_; i++) {
				if (buckets_[i]) {
					delete buckets_[i];
				}
			}
			delete[] buckets_;
		}

		// Helper method for equals()
		// Return true if an object in in a CwC array of objects of given size, else return false
		bool isInArr_(Object* needle, Object** haystack, size_t haystackLen) {
			for (size_t i = 0; i < haystackLen; i++) {
				if (haystack[i]->equals(needle)) {
					return true;
				}
			}
			return false;
		}

		/**
		 * @brief Determines if this map equals the given object. For two maps to be equal, they must
		 * contain the exact same key-value pairs, BUT do not need to share the same locations inside
		 * of the buckets.
		 * 
		 * @param other object to compare to
		 * @return equals or not
		 */
		virtual bool equals(Object* other) {
			if (this == other) return true;
			Map* otherMap = dynamic_cast<Map*>(other);
			if (!otherMap) {
				return false;
			}
			if (this->size_ != otherMap->size_) return false;
			// different values for capacity_ (the number of buckets) is okay
			
			Object** thisKeys = this->get_keys();
			Object** otherKeys = otherMap->get_keys();
			
			Object* curKey = NULL;
			for (size_t i = 0; i < this->size_; i++) {
				curKey = thisKeys[i];
				if (!isInArr_(curKey, otherKeys, otherMap->size_)) {
					delete otherKeys;
					delete thisKeys;
					return false;
				}
			}

			delete otherKeys;
			delete thisKeys;

			// Can't have duplicate keys in same map, so the above (length check and the for loop) should be enough to check equality

			Object** thisValues = this->get_values();
			Object** otherValues = otherMap->get_values();

			Object* curValue = NULL;
			for (size_t i = 0; i < this->size_; i++) {
				curValue = thisValues[i];
				if (!isInArr_(curValue, otherValues, otherMap->size_)) {
					delete otherValues;
					delete thisValues;
					return false;
				}
			}
			
			curValue = NULL;
			for (size_t i = 0; i < otherMap->size_; i++) {
				curValue = otherValues[i];
				if (!isInArr_(curValue, thisValues, this->size_)) {
					delete otherValues;
					delete thisValues;
					return false;
				}
			}
			delete otherValues;
			delete thisValues;

			// Duplicate values may be possible for different keys
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
		virtual size_t hash() {
			Object** keys = this->get_keys();
			Object** values = this->get_values();
			
			size_t hash = 0;
			for (size_t i = 0; i < this->size_; i++) {
				hash += reinterpret_cast<size_t>(keys[i]);
			}
			for (size_t i = 0; i < this->size_; i++) {
				hash += reinterpret_cast<size_t>(values[i]);
			}

			delete values;
			delete keys;
			return hash;
		}

		/**
		 * @brief Prints a representation of map to the console.
		 */
		/*virtual void print() {
			printf("Map:\n");
			for(size_t i = 0; i < size_; i++) {
				buckets_[i]->print();
				printf("\n");
			}
		}*/

		/**
		 * @brief Determines the number of mappings in this map.
		 * @return size of map
		 */
		virtual size_t size() {
			return size_;
		}
		
		// A helper method to insert a <K, V> pair into a specific bucket
		// Returns nullptr if there was no entry for that key previously, else
		//   overwrites the old value with val and returns the old value
		Object* insertIntoBucket_(size_t index, Object* key, Object* val) {
			Entry* bucket = this->buckets_[index];
			if (!bucket || bucket == nullptr) {
				this->buckets_[index] = new Entry(key, val);
				this->size_++;
				return nullptr;
			}
			
			Entry* prevEntry = nullptr;
			Entry* curEntry = bucket;
			while (curEntry && curEntry != nullptr) {
				if (curEntry->getKey()->equals(key)) {
					// already seen this key, replace the value
					Object* oldVal = curEntry->getValue();
					curEntry->setValue(val);
					return oldVal;
				}
				prevEntry = curEntry;
				curEntry = curEntry->getNext();
			}
			
			Entry* newEntry = new Entry(key, val);
			prevEntry->setNext(newEntry);
			this->size_++;
			return nullptr;
		}	
		
		/**
		 * Adds a mapping from key to val to the map. A key must be unique, if there is already a key
		 * of the same value in the map, the value will be overwritten by the new value.
		 * 
		 * NOTE: This function will mutate the Map
		 * 
		 * @param key Object key
		 * @param val Object value
		 * @return the old value from the given key, a nullptr is returned if no old value exists
		 */
		Object* put(Object* key, Object* val) {
			if (!key || !val) {
				return nullptr;
			}
			size_t hash = key->hash();
			size_t index = hash % capacity_;
			return this->insertIntoBucket_(index, key, val);
		}

		// Returns the Object* corresponding to the specified key in the specified (by index) bucket
		// nullptr if not found in that bucket
		Object* getFromBucket_(size_t index, Object* key) {
			Entry* bucket = this->buckets_[index];
			if (!bucket) {
				return nullptr;
			}
			
			Entry* curEntry = bucket;
			while (curEntry) {
				if (curEntry->getKey()->equals(key)) {
					return curEntry->getValue();
				}
				curEntry = curEntry->getNext();
			}
			return nullptr;
		}

		 /**
		 * Returns the object that the key maps to.
		 * 
		 * @param key Object unique key
		 * @return Object value for the key, a nullptr is returned if no value exists
		 */
		Object* get(Object* key) {
			if (!key) {
				return nullptr;
			}
			size_t hash = key->hash();
			size_t index = hash % capacity_;
			
			return this->getFromBucket_(index, key);
		}

		// A helper method for remove()
		// Removes an object, equality based on key, from the specific bucket
		// Returns nullptr if the key isn't found, returns the entry removed if it is
		Entry* removeFromBucket_(size_t index, Object* key) {
			Entry* bucket = this->buckets_[index];
			if (!bucket) {
				return nullptr;
			}
			
			Entry* prevEntry = nullptr;
			Entry* curEntry = bucket;
			while (curEntry) {
				if (curEntry->getKey()->equals(key)) {
					if (!prevEntry) {
						// deleting curEntry, the first item of the bucket
						this->buckets_[index] = nullptr;
						this->size_--;
						return curEntry;
					}
					// deleting curEntry, an item in the middle of the bucket
					prevEntry->setNext(curEntry->getNext());
					curEntry->setNext(nullptr);
					this->size_--;
					return curEntry;
				}
				prevEntry = curEntry;
				curEntry = curEntry->getNext();
			}
			// key not found
			return nullptr;
		}

		/**
		 * Removes the key value mapping from the map.
		 * 
		 * @param key Object key of mapping to remove
		 * @return value that was removed, a nullptr is returned if no value exists
		 */
		Object* remove(Object* key) {
			if (!key) {
				return nullptr;
			}
			size_t hash = key->hash();
			size_t index = hash % capacity_;
			
			Entry* removedEntry = this->removeFromBucket_(index, key);
			return removedEntry->getValue(); // delete key?
		}

		// Returns true if the bucket of given index contains the key
		bool bucketContainsKey_(size_t index, Object* key) {
			Entry* bucket = this->buckets_[index];
			if (!bucket) {
				return false;
			}
			
			Entry* curEntry = bucket;
			while (curEntry) {
				if (curEntry->getKey()->equals(key)) {
					return true;
				}
				curEntry = curEntry->getNext();
			}
			return false;
		}

		/**
		 * Determines if the map contains the given key.
		 * @param key Object key
		 * @return if the key is in the map
		 */
		bool contains_key(Object* key) {
			if (!key) {
				return false;
			}
			size_t hash = key->hash();
			size_t index = hash % capacity_;
			
			return this->bucketContainsKey_(index, key);
		}

		/**
		* Returns a new Object array of keys. This array will need to be freed in memory after use.
		* NOTE: If a map has no keys, it will return an empty Object**
		* @return new Object array of keys
		*/
		virtual Object** get_keys() {
			Object** keys = new Object*[this->size_];
			Entry* bucket = nullptr;
			int nextIdx = 0;
			for (size_t i = 0; i < this->capacity_; i++) {
				bucket = this->buckets_[i];
				if (bucket && bucket != nullptr) {
					nextIdx = bucket->addKeysToArr(keys, nextIdx);
				}
			}
			return keys;
		}

		/**
		* Returns a new Object array of values. This array will need to be freed in memory after use.
		* NOTE: If a map has no values, it will return an empty Object**
		* @return new Object array of values
		*/
		virtual Object** get_values() {
			Object** values = new Object*[this->size_];
			Entry* bucket = nullptr;
			int nextIdx = 0;
			for (size_t i = 0; i < this->capacity_; i++) {
				bucket = this->buckets_[i];
				if (bucket) {
					nextIdx = bucket->addValuesToArr(values, nextIdx);
				}
			}
			return values;
		}
};

/** 
 * Represents a map where the key is a string and the value is a object. Inherits from map.
 * Example: { "string_list: new StrList("hi", "bye"), "string": new String("hello") }
 * 
 * These functions are restricted to only allowing String->Object pairs, meaning that a MapStrObj
 * will only allow String->Object pairs.
 */
class MapStrObj : public Map {
    public:
		Map* helper_;

		/**
		 * Constructor
		 */
		MapStrObj() {
			this->helper_ = new Map();
		}
		
		/**
		 * Constructor to tailor performance of map.
		 * @param capacity number of buckets available to store different values.
		 */
		MapStrObj(size_t capacity) {
			this->helper_ = new Map();
		}

		/**
		 * Deconstructor
		 */
		~MapStrObj() {
			delete this->helper_;
		}

		/**
		 * Adds a mapping from key to val to the map. A key must be unique, if there is already a key
		 * of the same value in the map, the value will be overwritten by the new value.
		 * 
		 * NOTE: This function will mutate the Map
		 * 
		 * @param key String key
		 * @param val Object value
		 * @return the old value from the given key, a nullptr is returned if no old value exists
		 */
		Object* put(String* key, Object* val) {
			Object* prev = this->helper_->put(key, val);
			this->size_ = this->helper_->size();
			return prev;
		}
		
		/**
		 * Returns the object that the key maps to.
		 * 
		 * @param key String unique key
		 * @return Object value for the key, a nullptr is returned if no value exists
		 */
		Object* get(String* key) {
			return this->helper_->get(key);
		}

		/**
		 * Removes the key value mapping from the map.
		 * 
		 * @param key String key of mapping to remove
		 * @return Object value that was removed, a nullptr is returned if no value exists
		 */
		Object* remove(String* key) {
			Object* removed = this->helper_->remove(key);
			this->size_ = this->helper_->size();
			return removed;
		}

		/**
		 * Determines if the map contains the given key.
		 * @param key String key
		 * @return if the key is in the map
		 */
		bool contains_key(String* key) {
			return this->helper_->contains_key(key);
		}

		/**
		* Returns a new Object array of keys. This array will need to be freed in memory after use.
		* NOTE: If a map has no keys, it will return an empty Object**
		* @return new Object array of keys
		*/
		// Overrides Map.get_keys()
		Object** get_keys() {
			return this->helper_->get_keys();
		}	

		/**
		* Returns a new Object array of values. This array will need to be freed in memory after use.
		* NOTE: If a map has no values, it will return an empty Object**
		* @return new Object array of values
		*/
		// Overrides Map.get_values()
		Object** get_values() {
			return this->helper_->get_values();
		}
		
		/**
		 * @brief Determines the number of mappings in this map.
		 * @return size of map
		 */
		 // Overrides Map.size()
		size_t size() {
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
		bool equals(Object* other) {
			if (this == other) return true;
			MapStrObj* otherMap = dynamic_cast<MapStrObj*>(other);
			if (!otherMap) {
				return false;
			}
			if (this->size_ != otherMap->size_) return false;
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
		size_t hash() {
			return this->helper_->hash();
		}
};

/** 
 * Represents a map where the key is a string and the value is a string. Inherits from map.
 * 
 * These functions are restricted to only allowing String->String pairs, meaning that a MapStrStr
 * will only allow String->String pairs.
 */
class MapStrStr : public Map {
    public:
		Map* helper_;
    
		/**
		 * Constructor
		 */
		MapStrStr() : Map() {
			helper_ = new Map();
		}
		
		/**
		 * Constructor to optimize performance of map.
		 * @param capacity number of buckets available to store different values.
		 */
		MapStrStr(size_t capacity) : Map() {
			helper_ = new Map(capacity);
		}

		/**
		 * Deconstructor
		 */
		~MapStrStr() {
			delete this->helper_;
		}

		/**
		 * Adds a mapping from key to val to the map. A key must be unique, if there is already a key
		 * of the same value in the map, the value will be overwritten by the new value.
		 * 
		 * NOTE: This function will mutate the Map
		 * 
		 * @param key String key
		 * @param val String value
		 * @return the old value from the given key, a nullptr is returned if no old value exists
		 */
		String* put(String* key, String* val) {
			String* prev = dynamic_cast<String*>(this->helper_->put(key, val)); // safe because all values are String*
			this->size_ = this->helper_->size();
			return prev;
		}
		
		/**
		 * Returns the String that the key maps to.
		 * 
		 * @param key String unique key
		 * @return String value for the key, a nullptr is returned if no value exists
		 */
		String* get(String* key) {
			Object* value = this->helper_->get(key);
			return dynamic_cast<String*>(value); //safe because all values inserted as Strings
		}

		/**
		 * Removes the key value mapping from the map.
		 * 
		 * @param key String key of mapping to remove
		 * @return String value that was removed, a nullptr is returned if no value exists
		 */
		String* remove(String* key) {
			Object* value = this->helper_->remove(key);
			this->size_ = this->helper_->size();
			return dynamic_cast<String*>(value); //safe because all values inserted as Strings
		}
		
		/**
		 * Determines if the map contains the given key.
		 * @param key String key
		 * @return if the key is in the map
		 */
		bool contains_key(String* key) {
			return this->helper_->contains_key(key);
		}

		/**
		* Returns a new Object array of keys. This array will need to be freed in memory after use.
		* NOTE: If a map has no keys, it will return an empty Object**
		* @return new Object array of keys
		*/
		Object** get_keys() {
			return this->helper_->get_keys();
		}

		/**
		* Returns a new Object array of values. This array will need to be freed in memory after use.
		* NOTE: If a map has no values, it will return an empty Object**
		* @return new Object array of values
		*/
		Object** get_values() {
			return this->helper_->get_values();
		}
		
		/**
		 * @brief Determines the number of mappings in this map.
		 * @return size of map
		 */
		 // Overrides Map.size()
		size_t size() {
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
		bool equals(Object* other) {
			if (this == other) return true;
			MapStrStr* otherMap = dynamic_cast<MapStrStr*>(other);
			if (!otherMap) {
				return false;
			}
			if (this->size_ != otherMap->size_) return false;
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
		size_t hash() {
			return this->helper_->hash();
		}
};