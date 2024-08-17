#ifndef TRANCIVER_H
#define TRANCIVER_H

#include <QDataStream>
#include <QTcpSocket>


enum Tranciver_commmand
{
    comRegistration = 1,       // регистрация
    comErrorCreationID,        // клиент не зарегестрировался
    comRegistratiomYes,        // клиент зарегистрировался
    comNotDataBase,            // нет базы данных
    comBreakClient,            // клиент просит сбросить данные о себе

    // KNB  -------------------------------
    comSelectionPlayerKNB,  // клиент собирает группу для игры
    comGameStartKNB,        // клиенты готовы играть дальше
    comGameStart,                                   // возможно надо удалить !!!!!!!
    comOfferToJoinKNB,      // отправка всем клиентам приглашение на игру
    comGroupAssembledKNB,   // группа собрана стартуем
    comStopFindGroupsKNB,   // стоп поиск
    comLeaveGroupsKNB,      // клиент покинул игру
    comGroupDisbandedKNB,   // группа расформирована
    comClientLeaveGroupsKNB,    // команда клиенту что один игрок покинул группу (в группе больше 2)
    comToStepKNB,              // сколько прошло времени (команда для другого клиента)
    // сама игра
    comMotionKNB,       // игрок сделал ход (команда серверу)
    comMotionStartTimerKNB,     // запуск таймера ожидания хода (Команда клиенту)
    comNotDoneMotionKNB,        // противник не сделал ход
    comVictoryKNB,          // победа
    comDefeatKNB,           // поражение
    comDrawKNB,             // ничья
    comNextKNB,             // следующий раунд
    comDefeatNextKNB,       // пропуск рaунда
    comNextPlaYKNB,         // продолжение игры после пропуска раунда
    // -----------------------------------


    // общие команды для игры -----------------------------------
    comOfferToJoinTake,   // принял приглашение (команда на сервер)
    comOfferRejoction,      // отказался от игры (команда на сервер)
    comCloseInvitation,     // группа собрана закрываем приглашения
    comOpponentOffer,       // игрок принял приглашение (команда на клиент)
    comTimerMotionStartStopKNB,     // повторный запуск таймера после остановки (команда на клиент)
    comTimerMotionKNB,          // команда всем клиентам, что можно запскать таймер
    // -----------------------------------


    // группа -----------------------------------
    comOfferGroup,       // приглашение в группу
    comNotOfferGroup,    // клиент уже в группе
    comErrorOfferGroup,  // произощла ошибка
    comDelClientGroup,   // исключить из группы
    comDelClientGrouoGood,  // клиент успешно исключён из группы
    comDeleteGroup,         // группа расформированна
    comDelClientGroupError, // возникла ошибка при исключении клиента из группы
    comLeaveClientGroup,    // клиент покидает группу
    comDelTotalGroup,       // расформировать группу
    // -----------------------------------


    comPairFound,           // возможно надо удалить !!!!!!!
    comNoPair,              // возможно надо удалить !!!!!!!


    comCountPlayer,
    comCloseClient,         // отключение клиента
    comConnectUser,         // база данных


    // чат -------------------------------
    comGeneralChatOnOff,    // принимает ли клиент общие сообщения
    comPrivateChatOnOff,    // принимает ли клиент личные сообщения
    comMessagePush,         // общее сообщение отправить
    comMessagePushPrivat,   // личное сообщение отправить
    comMessagePushGroup,    // сообщение группы отправить
    comMessagePull,         // общее сообщение  принять
    comMessagePullPrivate,  // личное сообщение принять
    comMessagePullGroup,    // сообщение группы принять
    comMessagePrivateError,
    comListClient,
    comStatusClientChat,
    comTest,
};

QDataStream & operator<<(QDataStream &stream, Tranciver_commmand value);

QDataStream & operator>>(QDataStream &stream, Tranciver_commmand &value);

struct PackegeDate
{
    int playerId = -1;
    int countPlayer = -1;
    int opponentId = -1;

};

QDataStream & operator<<(QDataStream &stream, PackegeDate value);

QDataStream & operator>>(QDataStream &stream, PackegeDate &value);

/*
class Tranciver
{
    QByteArray arrBlock;

public:

    template <typename Fun, typename Arg, typename ... Args>
    void iteratePacket(const Fun &fun, Arg &&arg, Args&&... args)
    {
        fun(std::forward<Arg>(arg));
        iteratePacket(fun, std::forward<Args>(args)...);
    }


    template <typename CustomCommand, typename... Args>
    void createPacket(const CustomCommand command, const Args& ... arg)
    {
        //SenderLock.lock();
        QDataStream out(&arrBlock,QIODevice::Append);
        out.setVersion(QDataStream::Qt_5_11);
        out << quint16(0) << command;
        iteratePacket
                (
                    [&](auto &arg)
                    {

                        out << arg;
                    },
                    arg...
                );

//        out.device()->seek(0);
//    //        //qDebug() << "Total size to put " <<
//        (quint16)(storage.size() - sizeof(quint16));
//        out << (quint16)(storage.size() - sizeof(quint16));


        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        m_pTcpSocket->write(arrBlock);

    }
}; */





#endif // TRANCIVER_H
