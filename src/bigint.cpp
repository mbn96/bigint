#include <stdexcept>
#include "bigint.h"

#define ONE_U 0x1u
#define TEN_U 0xau
#define _8_bit_mask 0xffu

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
                    bs.removeAt(i);
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

    int Bigint::compare_unsigned(const Bigint &other) const
    {
        size_t msb_1, msb_2;
        msb_1 = get_msb();
        msb_2 = other.get_msb();

        if (msb_1 == msb_2)
        {
            size_t curr_bytes_index = bytes.getSize();
            if (curr_bytes_index == 0)
            {
                return 0;
            }
            --curr_bytes_index;
            uint8_t curr_last_byte_1, curr_last_byte_2;
            curr_last_byte_1 = bytes[curr_bytes_index];
            curr_last_byte_2 = other.bytes[curr_bytes_index];

            while ((curr_last_byte_1 == curr_last_byte_2) && (curr_bytes_index > 0))
            {
                --curr_bytes_index;
                curr_last_byte_1 = bytes[curr_bytes_index];
                curr_last_byte_2 = other.bytes[curr_bytes_index];
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

    bool Bigint::is_zero(const m_bytes &bs) const
    {
        return (bs.getSize() == 1) && (bs[0] == 0);
    }

    size_t Bigint::get_msb(const m_bytes &bs) const
    {
        size_t bytes_count = bs.getSize() - 1;
        uint8_t lastByte = bs[bytes_count];
        uint8_t msb = 7;
        for (; msb >= 0; msb--)
        {
            if (lastByte & (ONE_U << msb))
            {
                break;
            }
        }
        return (bytes_count << 3) + msb;
    }

    size_t Bigint::get_msb() const
    {
        return get_msb(bytes);
    }

    void Bigint::internal_add(m_bytes &res, const m_bytes &b) const
    {
        size_t a_len = res.getSize();
        size_t b_len = b.getSize();
        size_t bigger_len = a_len > b_len ? a_len : b_len;

        uint16_t temp_16 = 0;
        uint8_t temp_8 = 0;

        for (size_t i = 0; i < bigger_len; i++)
        {
            temp_16 = temp_8;
            if (i < a_len)
            {
                temp_16 += res[i];
            }
            else
            {
                res.append(0);
            }

            if (i < b_len)
            {
                temp_16 += b[i];
            }

            res[i] = temp_16 & _8_bit_mask;
            temp_8 = temp_16 >> 8;
        }

        if (temp_8)
        {
            res.append(ONE_U);
        }
    }

    // always assumes res > b.
    void Bigint::internal_sub(m_bytes &res, const m_bytes &b) const
    {
        size_t a_len = res.getSize();
        size_t b_len = b.getSize();

        uint16_t temp_16 = 0;
        uint8_t temp_8, next_carry = 0, curr_carry = 0;

        for (size_t i = 0; i < a_len; i++)
        {
            curr_carry = next_carry;
            next_carry = 0;
            temp_16 = res[i];
            if (curr_carry > temp_16)
            {
                temp_16 += 0x100u;
                next_carry++;
            }
            temp_16 -= curr_carry;

            if (i < b_len)
            {
                temp_8 = b[i];
                if (temp_8 > temp_16)
                {
                    temp_16 += 0x100u;
                    next_carry++;
                }
                temp_16 -= temp_8;
            }
            res[i] = temp_16;
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

    void Bigint::internal_left_shift(m_bytes &res, uint64_t shift) const
    {
        if (shift > 8)
        {
            throw std::invalid_argument("internal_left_shift been called with shift value greater than 8.");
        }
        else if (shift)
        {
            uint16_t temp_16 = 0;
            uint8_t curr_byte, temp_8 = 0;
            size_t len = res.getSize();

            for (size_t i = 0; i < len; i++)
            {
                temp_16 = res[i];
                temp_16 = temp_16 << shift;
                curr_byte = temp_16 & _8_bit_mask;
                curr_byte = curr_byte | temp_8;
                res[i] = curr_byte;
                temp_8 = temp_16 >> 8;
            }

            if (temp_8)
            {
                res.append(temp_8);
            }
        }
    }

    void Bigint::internal_right_shift(m_bytes &res, uint64_t shift) const
    {
        if (shift > 8)
        {
            throw std::invalid_argument("internal_left_shift been called with shift value greater than 8.");
        }
        else if (shift)
        {
            uint16_t temp_16 = 0;
            uint8_t curr_byte, temp_8 = 0;
            size_t len = res.getSize();
            shift = 8 - shift;

            for (std::int64_t i = len - 1; i >= 0; i--)
            {
                temp_16 = res[i];
                temp_16 = temp_16 << shift;
                curr_byte = (temp_16 & 0xff00u) >> 8;
                curr_byte = curr_byte | temp_8;
                res[i] = curr_byte;
                temp_8 = temp_16 & 0xffu;
            }
            trim_bytes(res);
        }
    }

    void Bigint::internal_multi(m_bytes &res, uint8_t b) const
    {
        if (b)
        {
            uint32_t temp_32 = 0;
            uint16_t temp_16 = 0;
            // uint8_t temp_8 = 0;

            size_t res_len = res.getSize();

            for (size_t i = 0; i < res_len; i++)
            {
                temp_32 = res[i];
                temp_32 *= b;
                temp_32 += temp_16;
                res[i] = temp_32 & _8_bit_mask;
                temp_16 = temp_32 >> 8;
            }

            if (temp_16)
            {
                res.append(temp_16 & _8_bit_mask);
                if (temp_16 & 0xff00u)
                {
                    res.append(temp_16 >> 8);
                }
            }
        }
        else
        {
            res.clear();
            res.append(0);
        }
    }

    void Bigint::internal_multi(m_bytes &res, const m_bytes &b) const
    {
        if (!(is_zero(res) || is_zero(b)))
        {
            m_bytes a(res);
            size_t b_len = b.getSize() - 1;
            res.clear();

            // // taking care of the first byte:
            // internal_multi(res, b[b_len]);
            // internal_left_shift(res, 8);

            // iterate bytes:
            for (size_t i = b_len; i > 0; i--)
            {
                if (b[i])
                {
                    m_bytes temp(a);
                    internal_multi(temp, b[i]);
                    internal_add(res, temp);
                }
                internal_left_shift(res, 8);
            }
            if (b[0])
            {
                internal_multi(a, b[0]);
                internal_add(res, a);
            }
        }
        else
        {
            res.clear();
            res.append(0);
        }
    }

    static size_t get_ms_byte(uint64_t num)
    {
        uint64_t temp = 0xffu;
        using std::printf;
        for (int i = 7; i > 0; i--)
        {
            if (num & (temp << (i * 8)))
            {
                return i;
            }
        }
        return 0;
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
            bytes.append((num >> (i * 8)) & _8_bit_mask);
        }
    }

    Bigint::Bigint(const m_bytes &bs, uint8_t sign) : bytes(bs), sign(sign) {}

    Bigint::~Bigint() {}

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
        strm << (num.sign ? " - " : "") << num.bytes;
        return strm;
    }

}