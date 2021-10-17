#ifndef BIGINT_H
#define BIGINT_H

#include "ArrayList.hpp"
#include <cstdint>
#include <string>

namespace MBN
{
    using uint8_t = std::uint8_t;
    using uint16_t = std::uint16_t;
    using uint32_t = std::uint32_t;
    using uint64_t = std::uint64_t;
    using m_bytes = ArrayList<uint8_t>;

    class Bigint
    {
    private:
        m_bytes bytes;
        uint8_t sign;

        int compare(const Bigint &other) const;
        int compare_unsigned(const Bigint &other) const;

        size_t get_msb() const;

        m_bytes &internal_add(const m_bytes &a, const m_bytes &b, m_bytes &res);
        m_bytes &internal_sub(const m_bytes &a, const m_bytes &b, m_bytes &res);
        m_bytes &internal_add_sub(const Bigint &a, const Bigint &b, m_bytes &res, uint8_t &sign);

        m_bytes &internal_left_shift(m_bytes &res, uint64_t shift);
        m_bytes &internal_right_shift(m_bytes &res, uint64_t shift);

        m_bytes &internal_multi(const m_bytes &a, const m_bytes &b, m_bytes &res);
        m_bytes &internal_div(const m_bytes &a, const m_bytes &b, m_bytes &res);

    public:
        Bigint(std::int64_t num);
        Bigint(std::uint64_t num, uint8_t sign);
        Bigint(const std::string &num);
        Bigint(const char *num);
        Bigint(const Bigint &other);
        Bigint(Bigint &&other);
        ~Bigint();

        friend void swap(Bigint &self, Bigint &other);
        friend std::ostream &operator<<(std::ostream &strm, const Bigint &num);

        Bigint &operator=(Bigint other);

        Bigint operator+(const Bigint &other) const;
        Bigint operator-(const Bigint &other) const;
        Bigint operator*(const Bigint &other) const;
        Bigint operator/(const Bigint &other) const;

        Bigint operator<<(uint64_t shift) const;
        Bigint operator>>(uint64_t shift) const;

        Bigint operator&(const Bigint &other) const;
        Bigint operator|(const Bigint &other) const;

        bool operator>(const Bigint &other) const;
        bool operator<(const Bigint &other) const;
        bool operator==(const Bigint &other) const;
        bool operator!=(const Bigint &other) const;
        bool operator>=(const Bigint &other) const;
        bool operator<=(const Bigint &other) const;
    };

} // namespace MBN

#endif //BIGINT_H