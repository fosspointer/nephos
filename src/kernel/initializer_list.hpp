#pragma once

namespace std
{
    template<class T>
    class initializer_list
    {
    public:
        typedef unsigned long size_t;
        typedef T value_type;
        typedef const T& reference;
        typedef const T& const_reference;
        typedef size_t size_type;
        typedef const T* iterator;
        typedef const T* const_iterator;
    private:
        iterator m_array;
        size_type m_length;

        constexpr initializer_list(const_iterator iterator, size_type length)
            :m_array(iterator), m_length(length)
        {}
    public:
        constexpr initializer_list() noexcept
            :m_array(0), m_length(0)
        {}

        constexpr size_type size() const noexcept { return m_length; }
        constexpr const_iterator begin() const noexcept { return m_array; }
        constexpr const_iterator end() const noexcept { return begin() + size(); }
    };
}