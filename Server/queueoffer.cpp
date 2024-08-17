#include "myserver.h"
#include "ui_myserver.h"


// очередь
void MyServer::fromQueus(QMap<int, Player>::iterator it)
{
    if (!it.value().queueOffer.isEmpty())
    {
        Offer offer;
        offer = it.value().queueOffer.front();
        it.value().queueOffer.pop_front();
        sendToClient(it.value().pClientSocket,
                     offer.command,
                     offer.id, offer.game);
    }

    if (it.value().countOffer > 0)
    {
        it.value().countOffer -= 1;
    }
}

void MyServer::eraseQueus(QMap<int, Player>::iterator it, QString str)
{
    QList<Offer>::iterator itQueue = it.value().queueOffer.begin();

    while (itQueue != it.value().queueOffer.end())
    {
        if (itQueue->game == str)
        {
            itQueue = it.value().queueOffer.erase(itQueue);
            it.value().countOffer -= 1;
        }
        else
        {
            ++itQueue;
        }
    }
}

void MyServer::eraseQueus(QMap<int, Player>::iterator it, int id, QString str)
{
    QList<Offer>::iterator itQueue = it.value().queueOffer.begin();

    while (itQueue != it.value().queueOffer.end())
    {
        if (itQueue->id == id && itQueue->game == str)
        {
            itQueue = it.value().queueOffer.erase(itQueue);
            it.value().countOffer -= 1;
        }
        else
        {
            ++itQueue;
        }
    }
}

void MyServer::sendToOfferClient(Tranciver_commmand command,
                                 QMap<int, Player>::iterator opponentPlayer,
                                 int id, QString strGame)
{
    opponentPlayer.value().countOffer += 1;

    if (opponentPlayer.value().countOffer <= 3)
    {
        sendToClient(opponentPlayer.value().pClientSocket, command,
                     id, strGame);
    }
    else    // создание очереди
    {
        Offer offer;
        offer.command = command;
        offer.id = id;
        offer.game = strGame;
        opponentPlayer.value().queueOffer.push_back(offer);
    }
}


