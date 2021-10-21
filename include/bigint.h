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
    using int64_t = std::int64_t;
    using m_bytes = ArrayList<uint32_t>;
    using std::string;

    class Bigint
    {
    private:
        m_bytes bytes;
        uint8_t sign;

        int compare(const Bigint &other) const;
        int compare_unsigned(const Bigint &other) const;
        int compare_unsigned(const m_bytes &a, const m_bytes &b) const;

        bool is_zero(const m_bytes &bs) const;

        size_t get_msb(const m_bytes &bs) const;
        size_t get_lsb(const m_bytes &bs) const;

        void internal_add(m_bytes &res, const m_bytes &b) const;
        void internal_sub(m_bytes &res, const m_bytes &b) const;
        m_bytes internal_add_sub(const Bigint &b, uint8_t b_sign, uint8_t &res_sign) const;

        void internal_or(m_bytes &res, const m_bytes &b) const;
        void internal_and(m_bytes &res, const m_bytes &b) const;

        void internal_left_shift(m_bytes &res, uint64_t shift) const;
        void internal_left_shift_long(m_bytes &res, uint64_t shift) const;

        void internal_right_shift(m_bytes &res, uint64_t shift) const;
        void internal_right_shift_long(m_bytes &res, uint64_t shift) const;

        void internal_shift_helper(m_bytes &res, uint64_t shift, bool left_shift) const;

        void internal_multi(m_bytes &res, const m_bytes &b) const;
        void internal_multi(m_bytes &res, uint32_t b) const;
        void internal_div(m_bytes &rem, const m_bytes &b, m_bytes &result, bool want_result) const;

        void internal_div_alter(m_bytes a, m_bytes b, m_bytes &result) const;

        Bigint(const m_bytes &bs, uint8_t sign);

    public:
        Bigint(std::int64_t num);
        Bigint(uint64_t num, bool sign);
        Bigint(const string &num);
        Bigint(const char *num);
        Bigint(const Bigint &other);
        Bigint(Bigint &&other);
        ~Bigint();

        friend void swap(Bigint &self, Bigint &other);
        friend std::ostream &operator<<(std::ostream &strm, const Bigint &num);

        size_t get_msb() const;
        size_t get_lsb() const;

        Bigint &operator=(Bigint other);

        Bigint operator+(const Bigint &other) const;
        Bigint operator-(const Bigint &other) const;
        Bigint operator*(const Bigint &other) const;
        Bigint operator/(const Bigint &other) const;
        Bigint operator%(const Bigint &other) const;

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

        string to_string() const;
        string to__decimal_string() const;
    };

} // namespace MBN

#endif //BIGINT_H