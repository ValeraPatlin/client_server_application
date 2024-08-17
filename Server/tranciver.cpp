#include "tranciver.h"

QDataStream & operator<<(QDataStream &stream, Tranciver_commmand value)
{
    stream << static_cast<int> (value);
    return stream;
}


QDataStream & operator>>(QDataStream &stream, Tranciver_commmand &value)
{
    int temp{};
    stream >> temp;

    value = static_cast<Tranciver_commmand>(temp);


     return stream;
}


QDataStream &operator<<(QDataStream &stream, PackegeDate value)
{
    stream << value.playerId;
    stream << value.countPlayer;

    return stream;
}

QDataStream &operator>>(QDataStream &stream, PackegeDate &value)
{
    stream >> value.playerId;
    stream >> value.countPlayer;
    stream >> value.opponentId;

    return stream;
}
