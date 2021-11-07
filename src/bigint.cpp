#include <stdexcept>
#include "bigint.h"

#define ONE_U 0x1u
#define TEN_U 0xau
#define _8_bit_mask 0xffu
#define _16_bit_mask 0xffffu
#define _32_bit_mask 0xffffffffu

namespace MBN
{

    // static Bigint ONE_BIG(0, 0);

    static void trim_bytes(m_bytes &bs)
    {
        size_t len = bs.getSize();
        if (len > 1)
        {
            for (int i = len - 1; i > 0; i--)
            {
                if (bs[i])
                {
                    break;
                }
                else
                {
                    bs.popLast_no_return();
                }
            }
        }
    }

    int Bigint::compare(const Bigint &other) const
    {
        if (sign == other.sign)
        {
            int u_comp = compare_unsigned(other);
            if (sign)
            {
                return -u_comp;
            }
            else
            {
                return u_comp;
            }
        }
        else if (sign)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }

    int Bigint::compare_unsigned(const m_bytes &a, const m_bytes &b) const
    {
        size_t msb_1, msb_2;
        msb_1 = get_msb(a);
        msb_2 = get_msb(b);

        if (msb_1 == msb_2)
        {
            size_t curr_bytes_index = a.getSize();
            if (curr_bytes_index == 0)
            {
                return 0;
            }
            --curr_bytes_index;
            uint32_t curr_last_byte_1, curr_last_byte_2;
            curr_last_byte_1 = a[curr_bytes_index];
            curr_last_byte_2 = b[curr_bytes_index];

            while ((curr_last_byte_1 == curr_last_byte_2) && (curr_bytes_index > 0))
            {
                --curr_bytes_index;
                curr_last_byte_1 = a[curr_bytes_index];
                curr_last_byte_2 = b[curr_bytes_index];
            }

            if (curr_last_byte_1 > curr_last_byte_2)
            {
                return -1;
            }
            else if (curr_last_byte_1 < curr_last_byte_2)
            {
                return 1;
            }
            else
            {
                return 0;
            }
        }
        else if (msb_1 > msb_2)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

    int Bigint::compare_unsigned(const Bigint &other) const
    {
        return compare_unsigned(bytes, other.bytes);

        // size_t msb_1, msb_2;
        // msb_1 = get_msb();
        // msb_2 = other.get_msb();

        // if (msb_1 == msb_2)
        // {
        //     size_t curr_bytes_index = bytes.getSize();
        //     if (curr_bytes_index == 0)
        //     {
        //         return 0;
        //     }
        //     --curr_bytes_index;
        //     uint8_t curr_last_byte_1, curr_last_byte_2;
        //     curr_last_byte_1 = bytes[curr_bytes_index];
        //     curr_last_byte_2 = other.bytes[curr_bytes_index];

        //     while ((curr_last_byte_1 == curr_last_byte_2) && (curr_bytes_index > 0))
        //     {
        //         --curr_bytes_index;
        //         curr_last_byte_1 = bytes[curr_bytes_index];
        //         curr_last_byte_2 = other.bytes[curr_bytes_index];
        //     }

        //     if (curr_last_byte_1 > curr_last_byte_2)
        //     {
        //         return -1;
        //     }
        //     else if (curr_last_byte_1 < curr_last_byte_2)
        //     {
        //         return 1;
        //     }
        //     else
        //     {
        //         return 0;
        //     }
        // }
        // else if (msb_1 > msb_2)
        // {
        //     return -1;
        // }
        // else
        // {
        //     return 1;
        // }
    }

    bool Bigint::is_zero(const m_bytes &bs) const
    {
        return (bs.getSize() == 1) && (bs[0] == 0);
    }

    static size_t get_lsByte(const m_bytes &bs)
    {
        size_t len = bs.getSize();
        for (size_t i = 0; i < len; i++)
        {
            if (bs[i])
            {
                return i;
            }
        }
        return 0;
    }

    size_t Bigint::get_lsb(const m_bytes &bs) const
    {
        size_t lsByteIndex = get_lsByte(bs);
        uint32_t lsByte = bs[lsByteIndex];

        int lsb = 0;
        for (; lsb < 32; lsb++)
        {
            if (lsByte & (ONE_U << lsb))
            {
                break;
            }
        }

        // if (lsb == 8)
        // {
        //     return 0;
        // }

        return (lsb == 32) ? 0 : ((lsByteIndex << 5) + lsb);

        // return (lsByteIndex << 3) + lsb;
    }

    size_t Bigint::get_msb(const m_bytes &bs) const
    {
        size_t bytes_count = bs.getSize() - 1;
        uint32_t lastByte = bs[bytes_count];
        int msb = 31;
        for (; msb > 0; msb--)
        {
            if (lastByte & (ONE_U << msb))
            {
                break;
            }
        }
        return (bytes_count << 5) + msb;
    }

    size_t Bigint::get_msb() const
    {
        return get_msb(bytes);
    }

    size_t Bigint::get_lsb() const
    {
        return get_lsb(bytes);
    }

    bool Bigint::is_pow_of_2(const m_bytes &bs) const
    {
        size_t lsb = get_lsb(bs), msb = get_msb(bs);
        return (lsb == msb) && lsb;
    }

    bool Bigint::is_pow_of_2() const
    {
        return is_pow_of_2(bytes);
    }

    void Bigint::internal_or(m_bytes &res, const m_bytes &b) const
    {
        size_t a_len = res.getSize();
        size_t b_len = b.getSize();
        size_t smaller_len = a_len > b_len ? b_len : a_len;

        if (a_len < b_len)
        {
            for (size_t i = 0; i < b_len; i++)
            {
                res.append(b[i]);
            }
        }

        uint32_t temp_32 = 0;

        for (size_t i = 0; i < smaller_len; i++)
        {
            temp_32 = res[i];
            res[i] = temp_32 | b[i];
        }
    }

    void Bigint::internal_and(m_bytes &res, const m_bytes &b) const
    {
        size_t a_len = res.getSize();
        size_t b_len = b.getSize();
        size_t smaller_len = a_len > b_len ? b_len : a_len;

        if (a_len > b_len)
        {
            size_t c = a_len - b_len;
            for (size_t i = 0; i < c; i++)
            {
                res.popLast_no_return();
            }
        }

        uint32_t temp_32 = 0;

        for (size_t i = 0; i < smaller_len; i++)
        {
            temp_32 = res[i];
            res[i] = temp_32 & b[i];
        }

        trim_bytes(res);
    }

    void Bigint::internal_add(m_bytes &res, const m_bytes &b) const
    {
        size_t a_len = res.getSize();
        size_t b_len = b.getSize();
        size_t bigger_len = a_len > b_len ? a_len : b_len;

        uint64_t temp_64 = 0;
        uint32_t temp_32 = 0;

        for (size_t i = 0; i < bigger_len; i++)
        {
            temp_64 = temp_32;
            if (i < a_len)
            {
                temp_64 += res[i];
            }
            else
            {
                res.append(0);
            }

            if (i < b_len)
            {
                temp_64 += b[i];
            }

            res[i] = temp_64 & _32_bit_mask;
            temp_32 = temp_64 >> 32;
        }

        if (temp_32)
        {
            res.append(ONE_U);
        }
    }

    // always assumes res > b.
    void Bigint::internal_sub(m_bytes &res, const m_bytes &b) const
    {
        static const uint64_t CARRY_ONE = (0x1lu << 32);

        size_t a_len = res.getSize();
        size_t b_len = b.getSize();

        uint64_t temp_64 = 0;
        uint32_t temp_32, next_carry = 0, curr_carry = 0;

        for (size_t i = 0; i < a_len; i++)
        {
            curr_carry = next_carry;
            next_carry = 0;
            temp_64 = res[i];
            if (curr_carry > temp_64)
            {
                // temp_64 += 0x100u;
                temp_64 += CARRY_ONE;

                next_carry++;
            }
            temp_64 -= curr_carry;

            if (i < b_len)
            {
                temp_32 = b[i];
                if (temp_32 > temp_64)
                {
                    // temp_64 += 0x100u;
                    temp_64 += CARRY_ONE;

                    next_carry++;
                }
                temp_64 -= temp_32;
            }
            res[i] = temp_64;
        }
        trim_bytes(res);
    }

    m_bytes Bigint::internal_add_sub(const Bigint &b, uint8_t b_sign, uint8_t &res_sign) const
    {
        m_bytes res;
        if (sign == b_sign)
        {
            // m_bytes res(this->bytes);
            // std::cout << "Here in before copy.\n";
            res = this->bytes;
            // std::cout << "Here in after copy.\n";
            res_sign = sign;
            internal_add(res, b.bytes);
            return res;
        }
        else
        {
            int comp_u = compare_unsigned(b);
            if (comp_u)
            {
                if (comp_u == 1)
                {
                    // m_bytes res(b.bytes);
                    res = b.bytes;
                    res_sign = b_sign;
                    internal_sub(res, this->bytes);
                    return res;
                }
                else
                {
                    // m_bytes res(this->bytes);
                    res = this->bytes;
                    res_sign = sign;
                    internal_sub(res, b.bytes);
                    return res;
                }
            }
            else
            {
                // m_bytes res(1);
                res.clear();
                res.append(0);
                res_sign = 0;
                return res;
            }
        }
    }

#pragma region // Comment-out
    // m_bytes &Bigint::internal_add(const m_bytes &a, const m_bytes &b, m_bytes &res) const
    // {
    //     size_t a_len = a.getSize();
    //     size_t b_len = b.getSize();
    //     size_t bigger_len = a_len > b_len ? a_len : b_len;

    //     uint16_t temp_16 = 0;
    //     uint8_t temp_8 = 0;

    //     for (size_t i = 0; i < bigger_len; i++)
    //     {
    //         temp_16 += temp_8;
    //         if (i < a_len)
    //         {
    //             temp_16 += a[i];
    //         }
    //         if (i < b_len)
    //         {
    //             temp_16 += b[i];
    //         }

    //         res.append(temp_16 & 0xffu);
    //         temp_8 = temp_16 >> 8;
    //     }

    //     if (temp_8)
    //     {
    //         res.append(ONE_U);
    //     }
    //     return res;
    // }
#pragma endregion

    void Bigint::internal_shift_helper(m_bytes &res, uint64_t shift, bool left_shift) const
    {
        size_t full_shifts = shift / 32;
        size_t partial_shift = shift % 32;

        if (left_shift)
        {
            if (full_shifts)
            {
                res.shift_out(full_shifts, 0);
                // std::cout << res << std::endl;
            }
            if (partial_shift)
            {
                // std::cout << res << std::endl;

                internal_left_shift(res, partial_shift);
            }
        }
        else
        {
            if (full_shifts)
            {
                res.shift_in(full_shifts);
                if (res.getSize())
                {
                    trim_bytes(res);
                }
                else
                {
                    res.append(0);
                }
                // std::cout << res << std::endl;
            }
            if (partial_shift)
            {
                internal_right_shift(res, partial_shift);
            }
        }
    }

    void Bigint::internal_left_shift(m_bytes &res, uint64_t shift) const
    {
        if (shift > 32)
        {
            throw std::invalid_argument("internal_left_shift been called with shift value greater than 32.");
        }
        else if (shift)
        {
            uint64_t temp_64 = 0;
            uint32_t curr_byte, temp_32 = 0;
            size_t len = res.getSize();

            for (size_t i = 0; i < len; i++)
            {
                temp_64 = res[i];
                temp_64 = temp_64 << shift;
                curr_byte = temp_64 & _32_bit_mask;
                curr_byte = curr_byte | temp_32;
                res[i] = curr_byte;
                temp_32 = temp_64 >> 32;
            }

            if (temp_32)
            {
                res.append(temp_32);
            }
        }
    }

    void Bigint::internal_right_shift(m_bytes &res, uint64_t shift) const
    {
        if (shift > 32)
        {
            throw std::invalid_argument("internal_right_shift been called with shift value greater than 32.");
        }
        else if (shift)
        {
            uint64_t temp_64 = 0;
            uint32_t curr_byte, temp_32 = 0;
            size_t len = res.getSize();
            shift = 32 - shift;

            for (std::int64_t i = len - 1; i >= 0; i--)
            {
                temp_64 = res[i];
                temp_64 = temp_64 << shift;

                // curr_byte = (temp_16 & 0xff00u) >> 8;
                curr_byte = temp_64 >> 32;

                curr_byte = curr_byte | temp_32;
                res[i] = curr_byte;
                temp_32 = temp_64 & _32_bit_mask;
            }
            trim_bytes(res);
        }
    }

    void Bigint::internal_multi(m_bytes &res, uint32_t b) const
    {
        if (b)
        {
            uint64_t temp_64 = 0;
            uint32_t temp_32 = 0;
            // uint8_t temp_8 = 0;

            size_t res_len = res.getSize();

            for (size_t i = 0; i < res_len; i++)
            {
                temp_64 = res[i];
                temp_64 *= b;
                temp_64 += temp_32;
                res[i] = temp_64 & _32_bit_mask;
                temp_32 = temp_64 >> 32;
            }

            if (temp_32)
            {
                // res.append(temp_32 & _8_bit_mask);
                // if (temp_16 & 0xff00u)
                // {
                //     res.append(temp_16 >> 8);
                // }

                res.append(temp_32);
            }
        }
        else
        {
            res.clear();
            res.append(0);
        }
    }

    static void set_bit(m_bytes &bs, size_t bit)
    {
        bs[bit / 32] |= (1u << (bit % 32));
    }

    void Bigint::internal_multi(m_bytes &res, const m_bytes &b) const
    {
        if (!(is_zero(res) || is_zero(b)))
        {
            int comp_u = compare_unsigned(res, b);

            m_bytes internal_b(b);
            m_bytes internal_a(res);
            m_bytes temp;

            res.clear();
            res.append(0);

            bool is_a_smaller = (comp_u == 1);
            m_bytes smaller = is_a_smaller ? internal_a : internal_b;
            static const Bigint two(2, 0);
            static const Bigint one(1, 0);

            size_t msb;
            while (compare_unsigned(smaller, two.bytes) <= 0)
            {
                msb = get_msb(smaller);
                m_bytes subtractor(smaller.getSize(), 0);
                set_bit(subtractor, msb);
                internal_sub(smaller, subtractor);

                if (is_a_smaller)
                {
                    temp = internal_b;
                }
                else
                {
                    temp = internal_a;
                }

                internal_shift_helper(temp, msb, true);
                internal_add(res, temp);
            }

            if (compare_unsigned(smaller, one.bytes) == 0)
            {
                if (is_a_smaller)
                {
                    temp = internal_b;
                }
                else
                {
                    temp = internal_a;
                }

                internal_add(res, temp);
            }
        }
        else
        {
            res.clear();
            res.append(0);
        }
    }

    // void Bigint::internal_multi(m_bytes &res, const m_bytes &b) const
    // {
    //     if (!(is_zero(res) || is_zero(b)))
    //     {
    //         m_bytes a(res);
    //         size_t b_len = b.getSize() - 1;
    //         res.clear();

    //         // // taking care of the first byte:
    //         // internal_multi(res, b[b_len]);
    //         // internal_left_shift(res, 8);

    //         // iterate bytes:
    //         for (size_t i = b_len; i > 0; i--)
    //         {
    //             if (b[i])
    //             {
    //                 m_bytes temp(a);
    //                 internal_multi(temp, b[i]);
    //                 internal_add(res, temp);
    //             }
    //             // internal_left_shift(res, 8);
    //             internal_shift_helper(res, 8, true);
    //         }
    //         if (b[0])
    //         {
    //             internal_multi(a, b[0]);
    //             internal_add(res, a);
    //         }
    //     }
    //     else
    //     {
    //         res.clear();
    //         res.append(0);
    //     }
    // }

    void Bigint::internal_div_alter(m_bytes a, m_bytes b, m_bytes &result) const
    {
        if (is_zero(b))
        {
            throw std::invalid_argument("Divisor is equal to zero(0).");
        }

        int comp_u = compare_unsigned(a, b);
        if (comp_u == -1)
        {
            result.append(0);
            // int64_t msb_diff = get_msb(a) - get_msb(b);
            // int64_t a_lsb = get_lsb(a);
            // int64_t min_shift = a_lsb < msb_diff ? a_lsb : msb_diff;

            // // shift result to left after division.
            // int64_t result_left_shift = 0;

            // if (min_shift)
            // {
            //     internal_shift_helper(a, min_shift, false);
            //     if ((comp_u = compare_unsigned(a, b)) == 1)
            //     {
            //         internal_left_shift(a, 1);
            //         --min_shift;
            //     }

            //     result_left_shift = min_shift;
            // }

            int64_t a_lsb = get_lsb(a);
            int64_t b_lsb = get_lsb(b);
            int64_t min_shift = a_lsb < b_lsb ? a_lsb : b_lsb;
            internal_shift_helper(a, min_shift, false);
            internal_shift_helper(b, min_shift, false);
            int64_t msb_diff = get_msb(a) - get_msb(b);

            static const Bigint big_one(1, 0);
            m_bytes internal_b(b);
            internal_shift_helper(internal_b, msb_diff, true);

            for (; msb_diff >= 0; msb_diff--)
            {
                // std::cout << "in div loop" << std::endl;
                // std::cout << rem << std::endl;
                // std::cout << internal_b << std::endl;
                // std::cout << result << std::endl;

                if ((comp_u = compare_unsigned(a, b)) == 1)
                {
                    // std::cout << "in small rem detect" << std::endl;

                    if (msb_diff /*|| result_left_shift*/)
                    {
                        internal_shift_helper(result, msb_diff /*+ result_left_shift*/, true);
                    }

                    return;
                }
                comp_u = compare_unsigned(a, internal_b);
                if (comp_u <= 0)
                {
                    internal_sub(a, internal_b);
                    internal_or(result, big_one.bytes);
                }
                if (msb_diff)
                {
                    internal_right_shift(internal_b, 1);
                    internal_left_shift(result, 1);
                }
            }
            /*
            if (result_left_shift)
            {
                internal_shift_helper(result, result_left_shift, true);
            }
            */
        }
        else if (comp_u)
        {
            result.append(0);
        }
        else
        {
            result.append(1);
        }
    }

    void Bigint::internal_div(m_bytes &rem, const m_bytes &b, m_bytes &result, bool want_result) const
    {
        if (is_zero(b))
        {
            throw std::invalid_argument("Divisor is equal to zero(0).");
        }

        int comp_u = compare_unsigned(rem, b);
        if (comp_u == -1)
        {
            // actual divide logic:
            m_bytes internal_b(b);
            int64_t msb_diff = get_msb(rem) - get_msb(b);
            static const Bigint big_one(1, 0);
            internal_shift_helper(internal_b, msb_diff, true);
            result.append(0);

            for (; msb_diff >= 0; msb_diff--)
            {
                // std::cout << "in div loop" << std::endl;
                // std::cout << rem << std::endl;
                // std::cout << internal_b << std::endl;
                // std::cout << result << std::endl;

                if ((comp_u = compare_unsigned(rem, b)) == 1)
                {
                    // std::cout << "in small rem detect" << std::endl;

                    if (msb_diff && want_result)
                    {
                        internal_shift_helper(result, msb_diff, true);
                    }

                    return;
                }
                comp_u = compare_unsigned(rem, internal_b);
                if (comp_u <= 0)
                {
                    internal_sub(rem, internal_b);
                    if (want_result)
                    {
                        internal_or(result, big_one.bytes);
                    }
                }
                if (msb_diff)
                {
                    internal_right_shift(internal_b, 1);
                    if (want_result)
                    {
                        internal_left_shift(result, 1);
                    }
                }
            }
        }
        else if (comp_u)
        {
            result.append(0);
        }
        else
        {
            rem.clear();
            rem.append(0);
            result.append(1);
        }
    }

    static size_t get_ms_byte(uint64_t num)
    {
        uint64_t temp = _32_bit_mask;
        using std::printf;
        for (int i = 1; i > 0; i--)
        {
            if (num & (temp << (i * 32)))
            {
                return i;
            }
        }
        return 0;
    }

    Bigint::Bigint(const Bigint &other) : bytes(other.bytes), sign(other.sign) {}

    Bigint::Bigint(Bigint &&other)
    {
        swap(*this, other);
    }

    Bigint::Bigint(uint64_t num, bool sign) : sign(sign)
    {
        // using std::cout;
        // using std::endl;
        // using std::printf;

        auto ms_byte = get_ms_byte(num);
        // cout << "this is msb: " << ms_byte << endl;
        // printf("this is num: %lx\n", num);
        for (size_t i = 0; i <= ms_byte; i++)
        {
            bytes.append((num >> (i * 32)) & _32_bit_mask);
        }
    }

    Bigint::Bigint(const m_bytes &bs, uint8_t sign) : bytes(bs), sign(sign) {}

    Bigint::Bigint(const string &num)
    {
        internal_string_constr(num.c_str(), num.length());
    }

    Bigint::Bigint(const char *num, uint64_t n)
    {
        internal_string_constr(num, n);
    }

    void Bigint::internal_string_constr(const char *num, uint64_t n)
    {
        // logic to convert base-10 string integer to binary bigInt
        if (*num == '-')
        {
            sign = 1;
            num++;
        }
        bytes.append(0);
        m_bytes temp(1, 0);

        for (; *(num + 1); num++)
        {
            if (*num >= '0' && *num <= '9')
            {
                temp[0] = (*num) - 48;
                internal_add(bytes, temp);
                internal_multi(bytes, 10);
            }
            else
            {
                throw std::invalid_argument("Only chars(0-9,-) are allowed.");
            }
        }

        if (*num >= '0' && *num <= '9')
        {
            temp[0] = (*num) - 48;
            internal_add(bytes, temp);
        }
        else
        {
            throw std::invalid_argument("Only chars(0-9,-) are allowed.");
        }
    }

    Bigint::~Bigint()
    {
        // std::cout << "Here in custom destructor Bigint.\n";
    }

    Bigint &Bigint::operator=(Bigint other)
    {
        swap(*this, other);
        return *this;
    }

    Bigint Bigint::operator+(const Bigint &other) const
    {
        uint8_t res_sign;
        m_bytes res = internal_add_sub(other, other.sign, res_sign);
        // std::cout << "Here in after return.\n";
        return Bigint(res, res_sign);
    }

    Bigint Bigint::operator-(const Bigint &other) const
    {
        uint8_t res_sign;
        auto res = internal_add_sub(other, other.sign ? 0 : 1, res_sign);
        return Bigint(res, res_sign);
    }

    Bigint Bigint::operator*(const Bigint &other) const
    {
        m_bytes res(bytes);
        internal_multi(res, other.bytes);
        return Bigint(res, sign == other.sign ? 0 : 1);
    }

    Bigint Bigint::operator/(const Bigint &other) const
    {
        // m_bytes rem(bytes);
        m_bytes result;
        internal_div_alter(bytes, other.bytes, result);
        // internal_div(rem, other.bytes, result, true);

        return Bigint(result, sign == other.sign ? 0 : 1);
    }

    //TODO: Needs some work...
    Bigint Bigint::operator%(const Bigint &other) const
    {
        m_bytes rem(bytes);
        m_bytes result;
        internal_div(rem, other.bytes, result, false);
        return Bigint(rem, sign == other.sign ? 0 : 1);
    }

    bool Bigint::operator>(const Bigint &other) const
    {
        return compare(other) == -1;
    }

    bool Bigint::operator<(const Bigint &other) const
    {
        return compare(other) == 1;
    }

    bool Bigint::operator==(const Bigint &other) const
    {
        return compare(other) == 0;
    }

    bool Bigint::operator!=(const Bigint &other) const
    {
        return compare(other) != 0;
    }

    bool Bigint::operator>=(const Bigint &other) const
    {
        int comp = compare(other);
        return comp <= 0;
    }

    bool Bigint::operator<=(const Bigint &other) const
    {
        int comp = compare(other);
        return comp >= 0;
    }

    std::ostream &operator<<(std::ostream &strm, const Bigint &num)
    {
        // strm << (num.sign ? " - " : "") << "Bits count: " << (num.bytes.getSize() * 32) << num.bytes;
        strm << "Bits count: " << (/*num.bytes.getSize() * 32*/ num.get_msb()) << ' ' << num.to__decimal_string();

        return strm;
    }

    string Bigint::to_string() const
    {

        // std::cout << "in to string" << std::endl;
        using std::to_string;
        string result;

        m_bytes rem(bytes);
        m_bytes res;
        static const Bigint ten(10, 0);

        do
        {
            // std::cout << "in string loop" << std::endl;
            res.clear();
            internal_div(rem, ten.bytes, res, true);
            result = ((char)(rem[0] + 48)) + result;
            rem = res;
            // std::cout << res << std::endl;
        } while (!is_zero(res));
        if (sign)
        {
            result = '-' + result;
        }
        return result;
    }

    string Bigint::to__decimal_string() const
    {
        using std::to_string;
        string result;
        uint32_t current;
        m_bytes rem(bytes);
        m_bytes res;

       

        static const Bigint BILLION(1000000000, 0);

        do
        {
            internal_div(rem, BILLION.bytes, res, true);
            current = rem[0];
            swap(rem, res);
            // rem = res;
            res.clear();
            for (int i = 0; i < 9; i++)
            {
                // sb.append((char)('0' + current % 10));
                result += (char)(48 + (current % 10));
                current /= 10;
                if (current == 0 && is_zero(rem))
                {
                    break;
                }
            }

        } while (!is_zero(rem));
         if (sign)
        {
            result += '-';
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    void swap(Bigint &self, Bigint &other)
    {
        // std::cout << "Here in custom swap Bigint.\n";
        using MBN::swap;
        swap(self.bytes, other.bytes);
        using std::swap;
        swap(self.sign, other.sign);
    }
}