#ifndef FREELISTSET_H
#define FREELISTSET_H

#include <iostream>
#include <array>

#include "macro/macrodebugassert.h"

namespace stdext {

template<typename T, std::size_t ChunkSize>
class freelist_set;

template<typename T>
class freelist_set_node
{
    template<typename Tc, std::size_t ChunkSize>
    friend class freelist_set;

    using data_type = typename std::aligned_storage<sizeof(T), alignof(T)>::type;

    template<typename...Args>
    void create(Args... args) { new(&data_) T(std::forward<Args...>(args)...); }

    void destroy() { reinterpret_cast<T*>(&data_)->~T(); }

    data_type data_;
    freelist_set_node* next_free_;

public:
    T &get() { return reinterpret_cast<T&>(data_); }
    const T &get_const() { return reinterpret_cast<const T&>(data_); }

    T *get_ptr() { return reinterpret_cast<T*>(&data_); }
    const T *get_ptr_const() { return reinterpret_cast<const T*>(&data_); }
};

// chunked free list array
template<typename T, std::size_t ChunkSize>
class freelist_set
{
public:
    using node = freelist_set_node<T>;

    freelist_set() : count_(0), max_count_(0)
    {
        entry_node_.next_free_ = &(entry_chunk_.nodes_[0]);
        /*for (auto it=entry_chunk_.nodes_.begin(); it<entry_chunk_.nodes_.end(); it++)
        {
            it->next_free_ = it+1; // point to the next node
        }*/
    }

    template<typename...Args>
    node* create(Args... args)
    {
        DEBUG_ASSERT(count_ < ChunkSize);
        node* new_node = get_first_free();
        std::cout << "creating node at:" << new_node << std::endl;
        new_node->create(std::forward<Args...>(args)...);
        count_++;
        max_count_ = count_ > max_count_ ? count_ : max_count_;
        return new_node;
    }

    inline bool is_active(node* n) { return n->next_free_ == &entry_node_; }

    void destroy(node* n)
    {
        if (is_active(n))
        {
            std::cout << "destroying node:"<< n << std::endl;
            n->destroy();
            n->next_free_ = entry_node_.next_free_;
            entry_node_.next_free_ = n;
            count_--;
        }
        else
        {
            std::cerr << "tried to destroy invalid node handle" << std::endl;
        }
    }

    node* get_by_offset(std::size_t offset)
    {
        node* out = &(entry_chunk_.nodes_[offset]);
        DEBUG_ASSERT(is_active(out));
        return out;
    }

    template<typename F>
    void for_all(F f)
    {
        for (std::size_t i=0; i<max_count_; i++)
        {
            node &n = entry_chunk_.nodes_[i];
            if (is_active(&n))
            {
                f(n.get());
            }
        }
    }


    template<typename F>
    void for_all_const(F f) const
    {
        for (std::size_t i=0; i<max_count_; i++)
        {
            const node &n = entry_chunk_.nodes_[i];
            if (is_active(&n))
            {
                f(n.get());
            }
        }
    }

private:
    node* get_first_free()
    {
        node* first_free = entry_node_.next_free_;
        entry_node_.next_free_ = first_free->next_free_;
        first_free->next_free_ = &entry_node_;
        return first_free;
    }


    struct chunk
    {
        chunk() // initialize nodes free list
        {
            for (auto it=nodes_.begin(); it<nodes_.end(); it++)
            {
                it->next_free_ = it+1; // point to the next node
            }
        }

        std::array<node, ChunkSize> nodes_;
    };

private:
    chunk entry_chunk_;
    std::size_t count_;
    std::size_t max_count_;
    node entry_node_;

    template<typename A, typename B, typename F, std::size_t NA, std::size_t NB>
    friend void freelist_set_zip(freelist_set<A, NA> &fls_a, freelist_set<B, NB> &fls_b, F f);

};


template<typename A, typename B, typename F, std::size_t NA, std::size_t NB>
void freelist_set_zip(freelist_set<A, NA> &fls_a, freelist_set<B, NB> &fls_b, F f)
{
    for (std::size_t i=0; i<std::min(fls_a.max_count_, fls_b.max_count_); i++)
    {
        freelist_set_node<A> &na = fls_a.entry_chunk_.nodes_[i];
        if (fls_a.is_active(&na))
        {
            freelist_set_node<B> &nb = fls_b.entry_chunk_.nodes_[i];

            DEBUG_ASSERT(fls_b.is_active(&nb));

            f(na.get(), nb.get());
        }
    }
}

}

#endif // FREELISTSET_H
