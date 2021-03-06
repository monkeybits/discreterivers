#ifndef FLAGS_H
#define FLAGS_H

#include <type_traits>

namespace stdext {

template<class FlagBits, FlagBits Default>
class Flags
{
    static_assert(std::is_enum<FlagBits>::value, "FlagBits must be enum type");
public:
    using Flag = FlagBits;

    Flags() : mFlags(Default) {}
    Flags(FlagBits flags) : mFlags(flags) {}

    inline static Flags combine(Flags flags1, Flags flags2)
    {
        return Flags(static_cast<FlagBits>(flags1.mFlags | flags2.mFlags));
    }


    inline bool checkFlag(FlagBits flag) const { return ( (mFlags & flag) == flag); } // PARENTHESIS NEEDED
    inline bool checkAny(FlagBits flag) const { return ( (mFlags & flag)) != 0; }

    inline void setFlag(FlagBits flag) { mFlags |= flag; }
    inline void clearFlag(FlagBits flag) { mFlags &= ~flag; }
    inline void toggleFlag(FlagBits flag) { mFlags ^= flag; }

    inline void clearAll() { mFlags = Default; }

private:
    unsigned char mFlags;
};

}

#endif // FLAGS_H
