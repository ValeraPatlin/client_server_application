#ifndef MYSERVER_H
#define MYSERVER_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QTcpServer>
#include <QDebug>
#include <QMap>
#include <QVector>
#include <QList>
#include <QRandomGenerator>
#include <QStringList>
#include <QFile>
#include <tuple>
#include "tranciver.h"


namespace Ui {
class MyServer;
}


struct Offer
{
    Tranciver_commmand command;
    QString game;
    int id{};
};


struct Player
{
    QTcpSocket* pClientSocket = nullptr;
    int id = -1;
    bool generalChat{};  // принимает ли игрок общие сообщения в чат
    bool privateChat{};  // принимает ли игрок личные сообщения в чат
    QString name;
    QString status;     // статус игрока (свододен, занят, играет в

    int countOffer = 0;    // количество приглашений
    QList<Offer> queueOffer;


    // KNB
    int countOpponentKNB = 0;
    bool gameKNB = false;   // играет ли угрок в KNB
    QVector<int> idOpponentKNB;
    QVector<int> idOppRaundNextKNB;
    QVector<int> idMotionKNB;
    bool motionKNB = false;    // сделал ли ход игрок
    bool droppedKNB = false;    // выбыл ли игрок
    QString answerKNB;
    int victoryPoints = 0;        // очки победы
    bool nextKNB = false;       // следующий раунд


    // group
    bool group_1 = false;
    bool group_2 = false;
    bool group_3 = false;
    QVector<int> idVecGroup_1;
    QVector<int> idVecGroup_2;
    QVector<int> idVecGroup_3;
};




class MyServer : public QWidget
{
    Q_OBJECT

    QTcpServer* m_ptcpServer;
    quint16 m_nNexyBlockSize{};

    QMap<int, Player> m_playersMap;

    //Player m_player;

    QStringList m_playerList;

    std::tuple<Player, bool> creatingPair(int id);    // создание пары для игры

    void sendToClient(QTcpSocket* pSocket, Tranciver_commmand command,
                      int number = -1, QString str = "",
                      PackegeDate packege_date = {});
    void sendToClient(QTcpSocket* pSocket,
                      Tranciver_commmand command,
                      QVector<int>& vec, QString str = "", int host = -1);

    int registrationServer();
    void countPlayer();
    void listClient();
    void writeStatusClient(int id, int status = -1);

    int responseCode(QString answerOne, QString answerTwo);

    // очередь ------------------
    void fromQueus(QMap<int, Player>::iterator it);
    void eraseQueus(QMap<int, Player>::iterator it, QString str);
    void eraseQueus(QMap<int, Player>::iterator it, int id, QString str);
    // отправка данных клиенту (или создание очереди)
    void sendToOfferClient(Tranciver_commmand command, QMap<int, Player>::iterator opponentPlayer,
                           int id, QString strGame);
    // ------------------------------------------

    void leaveGroups(int id, QStringList group = {});    // клиент покинул группу (любую группу)
    void leaveGroupsKNB(int id);    // клиент покинул группу KNB (вышел)

    void victoryPointsPlayer(QMap<int, Player>::iterator itOne, QMap<int, Player>::iterator itTwo);

    // подчет очков и определение победителя (или старт 2 раунда)
    void motionKNB(QMap<int, Player>::iterator mainPlayer);

    // подчет очков и определение ЕСЛИ 2 участника !!!
    void response(QMap<int, Player>::iterator mainPlayer);

    // очистка данных
    void clearDataPlayer(QMap<int, Player>::iterator mainPlayer, bool drop = false);

    // проверка состоит ли клиент в группе
    bool checkClientGroup(QMap<int, Player>::iterator opponentPlayer, QString& group);

    // отправка сообщения для группы
    void sendMessageGroup(QVector<int>& vec, QString& strMessage, int id);








    // база данных
    bool m_databaseGood{};

public:
    explicit MyServer(int nPort, QWidget *parent = 0);
    ~MyServer();

private:
    Ui::MyServer *ui;

public slots:
    virtual void slotNewConnection();
    void slotReadClient();
    void slotDeleteClient();

};

#endif // MYSERVER_H
