#pragma once

#include <list>
#include <unordered_map>

/// LRUCache decorator-like CRTP class inspired by Python's
/// functools.lru_cache decorator.
///
/// ValT is the type of FunctorT's compute method.
template<typename FunctorT, typename ValT>
class LRUCacheDecorator
{
    using KeyT = size_t;
    struct Entry
    {
        KeyT key;
        ValT val;
    };
    using QueueT = std::list<Entry>;
    using CacheT = std::unordered_map<KeyT, typename QueueT::iterator>;

  public:
    LRUCacheDecorator(size_t capacity = 20)
      : m_cap(capacity)
      , m_queue()
      , m_cache()
    {}

    template<typename... TArgs>
    ValT get(TArgs... args)
    {
        auto key = generate_hash(args...);
        auto val = get(key);
        if (val == nullptr) {
            auto ans = self().compute(args...);
            store(key, ans);

            return ans;
        }

        return *val;
    }

    void set_capacity(size_t capacity)
    {
        m_cap = capacity;
    }

    void reset()
    {
        m_cache.clear();
        m_queue.clear();
    }

  private:
    ValT* get(KeyT key)
    {
        auto cache_entry = m_cache.find(key);
        if (cache_entry != m_cache.end()) {
            touch(m_cache[key]);

            return &m_queue.front().val;
        }

        return nullptr;
    }

    void store(KeyT key, ValT val)
    {
        if (get(key) != nullptr) {
            // get(key) performs a touch op on our queue.
            m_queue.front().val = val;
        } else {
            if (m_cache.size() >= m_cap) {
                m_cache.erase(m_queue.back().key);
                m_queue.pop_back();
            }
            m_queue.push_front(Entry{ key, val });
            m_cache[key] = m_queue.begin();
        }
    }

    void touch(typename QueueT::iterator& q_iter)
    {
        if (q_iter != m_queue.begin()) {
            m_queue.splice(m_queue.begin(), m_queue, q_iter);
            m_cache[m_queue.front().key] = m_queue.begin();
        }
    }

    template<typename T>
    size_t generate_hash(T arg)
    {
        return std::hash<T>()(arg);
    }

    template<typename T, typename... Args>
    size_t generate_hash(T arg, Args... args)
    {
        return std::hash<T>()(arg) ^ generate_hash(args...);
    }

    /// Cast to a FunctorT to get access to FunctorT::compute()
    FunctorT& self()
    {
        return *static_cast<FunctorT*>(this);
    }

    size_t m_cap;
    QueueT m_queue;
    CacheT m_cache;
};
