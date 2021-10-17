#ifndef __BASEITERATOR_H__
#define __BASEITERATOR_H__

namespace MBN
{

    template <typename T>
    class BaseIterator
    {
    public:
        virtual ~BaseIterator() {}
        virtual bool hasNext() const = 0;
        virtual T &getNext() = 0;
    };

} // namespace MBN

#endif // __BASEITERATOR_H__