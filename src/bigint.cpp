#include "bigint.h"

#define ONE_U 0x1u
#define TEN_u 0x1010u

namespace MBN
{
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

    size_t Bigint::get_msb() const
    {
        size_t bytes_count = bytes.getSize() - 1;
        uint8_t lastByte = bytes[bytes_count];
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

    m_bytes &Bigint::internal_add(const m_bytes &a, const m_bytes &b, m_bytes &res)
    {
        size_t a_len = a.getSize();
        size_t b_len = b.getSize();
        size_t bigger_len = a_len > b_len ? a_len : b_len;

        uint16_t temp_16 = 0;
        uint8_t temp_8 = 0;

        for (size_t i = 0; i < bigger_len; i++)
        {
            temp_16 += temp_8;
            if (i < a_len)
            {
                temp_16 += a[i];
            }
            if (i < b_len)
            {
                temp_16 += b[i];
            }

            res.append(temp_16 & 0xffu);
            temp_8 = temp_16 >> 8;
        }

        if (temp_8)
        {
            res.append(ONE_U);
        }
        return res;
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

}