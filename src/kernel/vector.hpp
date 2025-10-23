#pragma once
#include "types.hpp"
#include "memory.hpp"
#include "assert.hpp"
#include "initializer_list.hpp"

template <typename T>
class Vector
{
public:
    using size_type = u32;
    Vector() noexcept
        :m_count(0ul)
    {
        const usize initial_count = 2ul;
        m_capacity = initial_count;
        m_data = reinterpret_cast<T*>(Memory::allocate(initial_count * sizeof(T)));
    }

    Vector(std::initializer_list<T> elements) noexcept
    {
        const usize initial_count = elements.size();
        m_capacity = m_count = initial_count;
        m_data = reinterpret_cast<T*>(Memory::allocate(initial_count * sizeof(T)));
        u32 i = 0;
        for(const auto& element: elements)
            m_data[i++] = element;
    }

    ~Vector() noexcept
    {
        Memory::free(m_data);
    }

    void reserve(usize count)
    {
        // assert(count > m_capacity, "cannot reserve less elements than the vector's capacity.");
        m_capacity = m_capacity > count? m_capacity: count;
        reallocate();
    }

    void resize(usize count)
    {
        assert(count > m_count, "new vector count cannot be smaller than the current count.");
        if(count > m_capacity)
        {
            m_capacity = count;
            reallocate();
        }
        m_count = count;
    }

    T& getBack() { return m_data[getCount() - 1ul]; }
    const T& getBack() const { return m_data[getCount() - 1ul]; }
    usize getCount() const { return m_count; }
    usize getCapacity() const { return m_capacity; }

    void push(const T& element)
    {
        if(m_count < m_capacity)
        {
            m_data[m_count++] = element;
            return;
        }

        m_capacity *= 2ul;
        reallocate();
        m_data[m_count++] = element;
    }

    void pop()
    {
        assert(m_count-- != 0ul, "cannot pop from empty vector.");
    }

    inline const T& operator[](usize index) const { return m_data[index]; }
    inline T& operator[](usize index) { return m_data[index]; }

    inline T* begin() { return m_data; }
    inline T* end() { return m_data + m_count; }

    inline const T* begin() const { return m_data; }
    inline const T* end() const { return m_data + m_count; }
protected:
    void reallocate()
    {
        T* old_data = m_data;
        m_data = reinterpret_cast<T*>(Memory::allocate(sizeof(T) * m_capacity));

        for(usize i{0ul}; i < m_count; ++i)
            m_data[i] = old_data[i];

        Memory::free(old_data);
    }
    T* m_data;
    usize m_count, m_capacity;
};