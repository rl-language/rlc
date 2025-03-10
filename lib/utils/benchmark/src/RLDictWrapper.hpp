#pragma once

#define RLC_GET_FUNCTION_DECLS
#define RLC_GET_TYPE_DECLS
#define RLC_GET_TYPE_DEFS
#include "dictionary.h"

#include <utility>
#include <cstddef>

template<typename KeyType, typename ValueType>
struct Entry {
    bool occupied;
    int64_t hash;
    KeyType key;
    ValueType value;
};

template<typename KeyType, typename ValueType>
class RLDictWrapper {
private:
    using DictType = typename std::conditional<
        std::is_same<KeyType, int>::value && std::is_same<ValueType, int>::value,
        DictIntInt,
        DictLargeKeyInt
    >::type;
    DictType dict;

public:
    using key_type = KeyType;
    using mapped_type = ValueType;
    using value_type = std::pair<const KeyType, ValueType>;
    using size_type = std::size_t;
    
    struct iterator {
        using iterator_category = std::forward_iterator_tag;
        using value_type = std::pair<const KeyType, ValueType>;
        using difference_type = std::ptrdiff_t;
        using pointer = value_type*;
        using reference = value_type&;
    };

    RLDictWrapper() {
        rl_m_init__Dict(&dict);
    }

    ~RLDictWrapper() {
        rl_m_drop__Dict(&dict);
    }

    // Prevent copying
    RLDictWrapper(const RLDictWrapper&) = delete;
    RLDictWrapper& operator=(const RLDictWrapper&) = delete;

    // Allow moving
    RLDictWrapper(RLDictWrapper&& other) noexcept = default;
    RLDictWrapper& operator=(RLDictWrapper&& other) noexcept = default;

    ValueType& operator[](const KeyType& key) {
        if (!contains(key)) {
            insert({key, ValueType{}});
        }
        return get_value(key);
    }

    std::pair<iterator, bool> insert(const value_type& value) {
        bool existed = contains(value.first);
        rl_m_insert__Dict_KeyType_ValueType(&dict, &value.first, &value.second);
        return {iterator{}, !existed};
    }

    iterator find(const KeyType& key) {
        return contains(key) ? iterator{} : iterator{};
    }

    bool contains(const KeyType& key) const {
        bool result = false;
        rl_m_contains__Dict_KeyType(&dict, &key, &result);
        return result;
    }

    size_type erase(const KeyType& key) {
        bool removed = false;
        rl_m_remove__Dict_KeyType(&dict, &key, &removed);
        return removed ? 1 : 0;
    }

    size_type size() const {
        size_t size_value = 0;
        rl_m_size__Dict(&dict, &size_value);
        return size_value;
    }

private:
    ValueType& get_value(const KeyType& key) {
        static ValueType value;
        rl_m_get__Dict_KeyType(&dict, &key, &value);
        return value;
    }
}; 