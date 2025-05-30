#ifndef DICTIONARY_CLASS
#define DICTIONARY_CLASS

#include <vector>
#include <string>
#include <initializer_list>


template <typename Key_T, typename Value_T>
class DictItem
{
public:
	DictItem();
	DictItem(Key_T key);
	DictItem(Key_T key, Value_T value);

	Key_T key;
	Value_T value;
};


template <typename Key_T, typename Value_T>
class Dictionary
{
public:
	Dictionary();
	Dictionary(std::initializer_list<DictItem<Key_T, Value_T>> list);

	bool has(Key_T key);
	bool add(Key_T key, Value_T value);
	bool remove(Key_T key);

	unsigned int getSize();
	int getIndex(Key_T key);
	DictItem<Key_T, Value_T>& getItemPtr(unsigned int index);

	void operator=(std::initializer_list<DictItem<Key_T, Value_T>> list);
	void operator=(Dictionary<Key_T, Value_T>& another);
	Value_T& operator[](Key_T key);

protected:
	std::vector<DictItem<Key_T, Value_T>> items;
};




template <typename Key_T, typename Value_T>
DictItem<Key_T, Value_T>::DictItem()
{
	key = Key_T();
	value = Value_T();
}

template <typename Key_T, typename Value_T>
DictItem<Key_T, Value_T>::DictItem(Key_T key)
{
	this->key = key;
	value = Value_T();
}

template <typename Key_T, typename Value_T>
DictItem<Key_T, Value_T>::DictItem(Key_T key, Value_T value)
{
	this->key = key;
	this->value = value;
}



template <typename Key_T, typename Value_T>
Dictionary<Key_T, Value_T>::Dictionary()
{
	items.clear();
}

template <typename Key_T, typename Value_T>
Dictionary<Key_T, Value_T>::Dictionary(std::initializer_list<DictItem<Key_T, Value_T>> list)
{
	*this = list;
}


template <typename Key_T, typename Value_T>
bool Dictionary<Key_T, Value_T>::has(Key_T key)
{
	return getIndex(key) > -1;
}

template <typename Key_T, typename Value_T>
bool Dictionary<Key_T, Value_T>::add(Key_T key, Value_T value)
{
	if (getIndex(key) > -1)
		return false;
	
	items.push_back(DictItem<Key_T, Value_T>(key, value));
	return true;
}

template <typename Key_T, typename Value_T>
bool Dictionary<Key_T, Value_T>::remove(Key_T key)
{
	int index = getIndex(key);
	if (index > -1)
		items.erase(items.begin() + index);
	return bool(index + 1);
}


template <typename Key_T, typename Value_T>
unsigned int Dictionary<Key_T, Value_T>::getSize()
{
	return items.size();
}

template <typename Key_T, typename Value_T>
int Dictionary<Key_T, Value_T>::getIndex(Key_T key)
{
	for (unsigned int i = 0; i < items.size(); i++)
		if (items[i].key == key)
			return i;
	return -1;
}

template <typename Key_T, typename Value_T>
DictItem<Key_T, Value_T>& Dictionary<Key_T, Value_T>::getItemPtr(unsigned int index)
{
	//if (index >= items.size())
	//	items.resize(index + 1);
	return items[index];
}


template <typename Key_T, typename Value_T>
void Dictionary<Key_T, Value_T>::operator=(std::initializer_list<DictItem<Key_T, Value_T>> list)
{
	items.resize(list.size());
	for (unsigned int i = 0; i < list.size(); i++)
		items[i] = *(list.begin() + i);
}

template <typename Key_T, typename Value_T>
void Dictionary<Key_T, Value_T>::operator=(Dictionary<Key_T, Value_T>& another)
{
	items.resize(another.getSize());
	std::copy(items.begin(), items.end(), another.items.begin());
}

template <typename Key_T, typename Value_T>
Value_T& Dictionary<Key_T, Value_T>::operator[](Key_T key)
{
	if (add(key, Value_T()))
		return items.back().value;
	return items[getIndex(key)].value;
}

#endif //DICTIONARY_CLASS