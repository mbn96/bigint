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