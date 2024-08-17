#include "myserver.h"
#include "ui_myserver.h"



void MyServer::slotReadClient()
{
    qDebug() << "\n ----------------------------- \n"
             << Q_FUNC_INFO;

    QTcpSocket* pClientSocket = dynamic_cast<QTcpSocket*>(sender());
    QDataStream in(pClientSocket);
    in.setVersion(QDataStream::Qt_5_11);

    while(true)
    {
        if (!m_nNexyBlockSize)
        {
            if (pClientSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> m_nNexyBlockSize;
        }

        if (pClientSocket->bytesAvailable() < m_nNexyBlockSize)
        {
            qDebug() << "ERROR !!" << pClientSocket->bytesAvailable() << m_nNexyBlockSize;
            break;
        }
        m_nNexyBlockSize = 0;

        Tranciver_commmand command;
        int id{};
        in >> command >> id;

        qDebug() << "com: " << command;

        switch (command)
        {
            case comRegistration:
            {
                int status;
                bool genChat, getPrivateChat;
                in >> status >> genChat >> getPrivateChat;

                int id = registrationServer();
                qDebug() << "id: " << id;

                if (id != -1)
                {
                    ui->te_info->append("client:  " + QString::number(id));

                    Player m_player;
                    m_player.id = id;
                    m_player.pClientSocket = pClientSocket;
                    m_player.generalChat = genChat;
                    m_player.privateChat = getPrivateChat;
                    m_playersMap.insert(id, m_player);

                    sendToClient(pClientSocket, comRegistratiomYes, id);
                    countPlayer();
                    writeStatusClient(id, status);
                }
                else
                {
                    sendToClient(pClientSocket, comErrorCreationID);
                }
                break;
            }

            // удалить или заменить
            case comGameStart:
            {
                auto values = creatingPair(id);
                Player playerOpponent = std::get<0>(values);
                bool pairGood = std::get<1>(values);

                if (pairGood)
                {
                    sendToClient(pClientSocket, comPairFound, playerOpponent.id);
                    sendToClient(playerOpponent.pClientSocket,
                                 comPairFound, playerOpponent.countOpponentKNB);
                }
                else
                {
                    sendToClient(pClientSocket, comNoPair);
                }
                break;
            }

            // клиент закрывается
            case comCloseClient:
            {
                leaveGroups(id);
                m_playersMap.remove(id);
                countPlayer();
                listClient();
                break;
            }

            // сброс данных о клиенте
            case comBreakClient:
            {
                leaveGroupsKNB(id);
                listClient();
                auto mainPlayer = m_playersMap.find(id);
                mainPlayer.value().gameKNB = false;
                mainPlayer.value().countOpponentKNB = 0;
                mainPlayer.value().idOpponentKNB.clear();
                mainPlayer.value().idMotionKNB.clear();
                mainPlayer.value().answerKNB.clear();
                mainPlayer.value().motionKNB = false;
                mainPlayer.value().droppedKNB = false;
                mainPlayer.value().victoryPoints = 0;
                mainPlayer.value().nextKNB = false;
                break;
            }

            // база данных
            case comConnectUser:        // доделать !!!
            {
                QString login;
                QString passwd;

                in >> login >> passwd;

                qDebug() << "login:  " << login
                         << "\npasswd:  " << passwd;

                if (m_databaseGood)
                {
                    //
                }
                else
                {
                    sendToClient(pClientSocket, comNotDataBase);
                }

                break;
            }


            // ------------------------------------------------
            // чат
            case comGeneralChatOnOff:
            {
                bool generalChat{};
                in >> generalChat;

                auto mainPlayer = m_playersMap.find(id);
                mainPlayer.value().generalChat = generalChat;

                listClient();
                break;
            }

            case comPrivateChatOnOff:
            {
                bool privateChat{};
                in >> privateChat;
                auto mainPlayer = m_playersMap.find(id);
                mainPlayer.value().privateChat = privateChat;
                break;
            }

            case comMessagePush:    // сообшение на клиент
            {
                QString strMessage;
                in >> strMessage;

                for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
                {
                    if (it->generalChat)
                    {
                        sendToClient(it->pClientSocket, comMessagePull, id, strMessage);
                    }
                }
                break;
            }

            case comMessagePushPrivat:      // личные сообщения
            {
                int idOpp{};
                QString strMessage;
                in >> idOpp >> strMessage;

                auto oppPlayer = m_playersMap.find(idOpp);

                if (oppPlayer.value().privateChat && oppPlayer != m_playersMap.end())
                {
                    sendToClient(oppPlayer.value().pClientSocket,
                                 comMessagePullPrivate, id, strMessage);
                }
                else
                {
                    auto mainPlayer = m_playersMap.find(id);
                    sendToClient(mainPlayer.value().pClientSocket, comMessagePrivateError);
                }
                break;
            }

            case comMessagePushGroup:       // сообщения для группы
            {
                QString grop, strMessage;
                in >> grop >> strMessage;

                auto mainPlayer = m_playersMap.find(id);

                if (grop == "group#1")
                {
                    sendMessageGroup(mainPlayer.value().idVecGroup_1, strMessage,
                                     mainPlayer.value().id);
                }
                else if (grop == "group#2")
                {
                    sendMessageGroup(mainPlayer.value().idVecGroup_2, strMessage,
                                     mainPlayer.value().id);
                }
                else if (grop == "group#3")
                {
                    sendMessageGroup(mainPlayer.value().idVecGroup_3, strMessage,
                                     mainPlayer.value().id);
                }
                else if (grop == "KNB")
                {
                    sendMessageGroup(mainPlayer.value().idOpponentKNB, strMessage,
                                     mainPlayer.value().id);
                }
                break;
            }

            case comStatusClientChat:
            {
                int statusClient;
                in >> statusClient;

                writeStatusClient(id, statusClient);
                break;
            }
            // --------------------------------------------------




            // --------------------------------------------------
            // KNB
            case comSelectionPlayerKNB:
            {
                QString strGame;
                int countPlayer;
                in >> countPlayer >> strGame;

                auto mainPlayer = m_playersMap.find(id);
                mainPlayer.value().countOpponentKNB = --countPlayer;

                qDebug() << "countPlayer: "
                         << mainPlayer.value().countOpponentKNB
                         << "countOffer: " << mainPlayer.value().countOffer;

                // тут надо сделать так что бы в зависимости от игры
                // определялась команда
                for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
                {
                    if (it.key() != id
                            && !it.value().gameKNB      // !!!!!!!!!
                            && it.value().status == "готов играть")
                    {
                        sendToOfferClient(comOfferToJoinKNB, it, id, strGame);
                    }
                }
                break;
            }

            // клиент принял приглашение
            case comOfferToJoinTake:
            {
            // тут считывается id того клиента который отправил всем приглашение
                int idOpponent;
                QString gameOffer;
                in >> idOpponent >> gameOffer;

                qDebug() << "idOpponent:  " << idOpponent << gameOffer;

                auto mainPlayer = m_playersMap.find(idOpponent);

                if (gameOffer == "KNB")
                {
                    mainPlayer.value().gameKNB = true;
                    mainPlayer.value().countOpponentKNB -= 1;
                    mainPlayer.value().idOpponentKNB.push_back(id);

                    sendToClient(mainPlayer.value().pClientSocket,
                                 comOpponentOffer,
                                 mainPlayer.value().idOpponentKNB,
                                 gameOffer);

                    for (auto itId : mainPlayer.value().idOpponentKNB)
                    {
                        auto opponentPlayer = m_playersMap.find(itId);
                        opponentPlayer.value().idOpponentKNB.clear();
                        opponentPlayer.value().idOpponentKNB.push_back(idOpponent);
                        opponentPlayer.value().gameKNB = true;      // игрок в группе

                        // удаляем из очереди элементы с одинаковой игрой
                        eraseQueus(opponentPlayer, gameOffer);
                        fromQueus(opponentPlayer);

                        for (auto elem : mainPlayer.value().idOpponentKNB)
                        {
                            if (elem != itId)
                            {
                                opponentPlayer.value().idOpponentKNB.push_back(elem);
                            }
                        }

                        // отправляем список (вектор) всем участникам
                        sendToClient(opponentPlayer.value().pClientSocket,
                                     comOpponentOffer,
                                     opponentPlayer.value().idOpponentKNB, gameOffer);
                    }

// если количество желаемых противников = 0, то значит группа собрана, а значит:
                    if (mainPlayer.value().countOpponentKNB < 1 && gameOffer == "KNB")
                    {
                        // закрываем все приглашения
                        for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
                        {
                            sendToClient(it.value().pClientSocket, comCloseInvitation,
                                         idOpponent, gameOffer);
                        }

                        // отправляем команду на старт игры всем участникам группы
                        sendToClient(mainPlayer.value().pClientSocket, comGroupAssembledKNB);
                        for (auto elem : mainPlayer.value().idOpponentKNB)
                        {
                            auto opponentPlayer = m_playersMap.find(elem);
                            sendToClient(opponentPlayer.value().pClientSocket,
                                         comGroupAssembledKNB);
                        }

                        mainPlayer.value().gameKNB = true;
                        listClient();
                    }
}
                else if (gameOffer == "group#1")   // группа 1
                {
                    mainPlayer.value().group_1 = true;
                    mainPlayer.value().idVecGroup_1.push_back(id);

                    // отправка данных хосту
                    sendToClient(mainPlayer.value().pClientSocket,
                                 comOpponentOffer,
                                 mainPlayer.value().idVecGroup_1,
                                 gameOffer);

                    // создания списка участников группы
                    for (auto itId : mainPlayer.value().idVecGroup_1)
                    {
                        auto oppPlayer = m_playersMap.find(itId);
                        oppPlayer.value().idVecGroup_1.clear();
                        oppPlayer.value().idVecGroup_1.push_back(idOpponent);
                        oppPlayer.value().group_1 = true;

                        // удаляем из очереди элементы с одинаковой игрой
                        eraseQueus(oppPlayer, gameOffer);
                        fromQueus(oppPlayer);

                        for (auto elem : mainPlayer.value().idVecGroup_1)
                        {
                            if (elem != itId)
                            {
                                oppPlayer.value().idVecGroup_1.push_back(elem);
                            }
                        }

                        // отправляем список (вектор) всем участникам
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comOpponentOffer,
                                     oppPlayer.value().idVecGroup_1,
                                     gameOffer, mainPlayer.value().id);
                    }
                }
                else if (gameOffer == "group#2")   // группа 2
                {
                    mainPlayer.value().group_2 = true;
                    mainPlayer.value().idVecGroup_2.push_back(id);

                    // отправка данных хосту
                    sendToClient(mainPlayer.value().pClientSocket,
                                 comOpponentOffer,
                                 mainPlayer.value().idVecGroup_2,
                                 gameOffer);

                    // создания списка участников группы
                    for (auto itId : mainPlayer.value().idVecGroup_2)
                    {
                        auto oppPlayer = m_playersMap.find(itId);
                        oppPlayer.value().idVecGroup_2.clear();
                        oppPlayer.value().idVecGroup_2.push_back(idOpponent);
                        oppPlayer.value().group_2 = true;

                        // удаляем из очереди элементы с одинаковой игрой
                        eraseQueus(oppPlayer, gameOffer);
                        fromQueus(oppPlayer);

                        for (auto elem : mainPlayer.value().idVecGroup_2)
                        {
                            if (elem != itId)
                            {
                                oppPlayer.value().idVecGroup_2.push_back(elem);
                            }
                        }

                        // отправляем список (вектор) всем участникам
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comOpponentOffer,
                                     oppPlayer.value().idVecGroup_2,
                                     gameOffer, mainPlayer.value().id);
                    }
                }
                else if (gameOffer == "group#3")   // группа 3
                {
                    mainPlayer.value().group_3 = true;
                    mainPlayer.value().idVecGroup_3.push_back(id);

                    // отправка данных хосту
                    sendToClient(mainPlayer.value().pClientSocket,
                                 comOpponentOffer,
                                 mainPlayer.value().idVecGroup_3,
                                 gameOffer);

                    // создания списка участников группы
                    for (auto itId : mainPlayer.value().idVecGroup_3)
                    {
                        auto oppPlayer = m_playersMap.find(itId);
                        oppPlayer.value().idVecGroup_3.clear();
                        oppPlayer.value().idVecGroup_3.push_back(idOpponent);
                        oppPlayer.value().group_3 = true;

                        // удаляем из очереди элементы с одинаковой игрой
                        eraseQueus(oppPlayer, gameOffer);
                        fromQueus(oppPlayer);

                        for (auto elem : mainPlayer.value().idVecGroup_3)
                        {
                            if (elem != itId)
                            {
                                oppPlayer.value().idVecGroup_3.push_back(elem);
                            }
                        }

                        // отправляем список (вектор) всем участникам
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comOpponentOffer,
                                     oppPlayer.value().idVecGroup_3,
                                     gameOffer, mainPlayer.value().id);
                    }
                }
                break;
            }

            // клиент отказался от приглашения
            case comOfferRejoction:
            {
                auto mainPlayer = m_playersMap.find(id);
                fromQueus(mainPlayer);
                break;
            }

// поиск остановлен, в группе НЕТ участников, тогда просто отсылаем команду о закрытии приглашений
            // если в группе есть участники то начинаем игру с ними
            case comStopFindGroupsKNB:
            {
                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer.value().countOpponentKNB > 0)
                {
                    for (auto elem : mainPlayer.value().idOpponentKNB)
                    {
                        auto opponentPlayer = m_playersMap.find(elem);
                        sendToClient(opponentPlayer.value().pClientSocket, comStopFindGroupsKNB);
                    }
                }
                mainPlayer.value().countOpponentKNB = 0;

                for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
                {

                    sendToClient(it.value().pClientSocket, comCloseInvitation, id , QString("KNB"));
                    eraseQueus(it, id, QString("KNB"));
                    fromQueus(it);

                }
                break;
            }

            // покинуть группу
            case comLeaveGroupsKNB:
            {
                leaveGroupsKNB(id);
                listClient();
                break;
            }

            // повторный запуск таймера при остановки
            // команда отправляется всем клиентам, что этот клиент продолжает играть
            case comTimerMotionStartStopKNB:
            {
                auto mainPlayer = m_playersMap.find(id);

                if (!mainPlayer.value().droppedKNB)
                {
                    for (auto elem : mainPlayer.value().idOpponentKNB)
                    {
                        auto opponentPlayer = m_playersMap.find(elem);
                        sendToClient(opponentPlayer.value().pClientSocket,
                                     comTimerMotionStartStopKNB,
                                     id);
                    }
                }
                break;
            }
            // здесь, все клинты согласились играть дальше и таймер запускается
            case comTimerMotionKNB:
            {
                int count = 0;

                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer.value().motionKNB || mainPlayer.value().droppedKNB)
                {
                    ++count;
                }

                for (auto elem : mainPlayer.value().idOpponentKNB)
                {
                    auto opponentPlayer = m_playersMap.find(elem);
                    if (opponentPlayer.value().motionKNB || opponentPlayer.value().droppedKNB)
                    {
                        ++count;
                    }
                }

                if (count == mainPlayer.value().idOpponentKNB.size() + 1)
                {
                    // если участников всего 2
                    if (mainPlayer.value().idOpponentKNB.size() == 1)
                    {
                        response(mainPlayer);
                    }
                    else        // участников больше 2
                    {
                        // подчет очков и определение победителя (или старт 2 раунда)
                        motionKNB(mainPlayer);
                    }
                }
                else
                {
                    sendToClient(mainPlayer.value().pClientSocket, comTimerMotionKNB, id);
                    for (auto elem : mainPlayer.value().idOpponentKNB)
                    {
                        auto opponentPlayer = m_playersMap.find(elem);
                        sendToClient(opponentPlayer.value().pClientSocket, comTimerMotionKNB, id);
                    }
                }

                break;
            }

            case comGameStartKNB:
            {
                auto mainPlayer = m_playersMap.find(id);
                sendToClient(mainPlayer.value().pClientSocket, comGroupAssembledKNB);
                for (auto elem : mainPlayer.value().idOpponentKNB)
                {
                    auto opponentPlayer = m_playersMap.find(elem);
                    sendToClient(opponentPlayer.value().pClientSocket, comGroupAssembledKNB);
                }
                break;
            }

            // ------- игрок сделал ход -------------------------------------------
            case comMotionKNB:
            {
                QString str;
                in >> str;
                auto mainPlayer = m_playersMap.find(id);
                mainPlayer.value().motionKNB = true;
                mainPlayer.value().answerKNB = str;

                bool flagMotion = true;

                ui->te_info->append(QString::number(id) + "  " + str + "  flag: " +
                                     QString::number(flagMotion));

                for (auto elem : mainPlayer.value().idOpponentKNB)
                {
                    auto opponentPlayer = m_playersMap.find(elem);

                    // если игрок не сделал ход и не выбыл
                    if (!opponentPlayer.value().motionKNB
                            && !opponentPlayer.value().droppedKNB)
                    {
                        sendToClient(opponentPlayer.value().pClientSocket,
                                     comMotionStartTimerKNB,
                                     mainPlayer.value().id);
                        flagMotion = false;
                    }
                }

                ui->te_info->append(QString::number(id) + "  " + str + "  flag: " +
                                     QString::number(flagMotion));

                // если все участники сделали ход
                if (flagMotion)
                {
                    // если участников всего 2
                    if (mainPlayer.value().idOpponentKNB.size() == 1)
                    {
                        response(mainPlayer);
                    }
                    else        // участников больше 2
                    {
                        // подчет очков и определение победителя (или старт 2 раунда)
                        motionKNB(mainPlayer);
                    }
                }
                break;
            }

            case comNotDoneMotionKNB:   // противник не сделал ход
            {
                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer.value().idOpponentKNB.size() > 1)
                {
                    mainPlayer.value().motionKNB = false;
                    mainPlayer.value().droppedKNB = true;
                    mainPlayer.value().victoryPoints = -1;

                    // подчет очков и определение победителя (или старт 2 раунда)
                    motionKNB(mainPlayer);
                }
                else
                {
                    // если два игрока то сразу отправить сообшение победителю
                    for (auto elem : mainPlayer.value().idOpponentKNB)
                    {
                        auto opponentPlayer = m_playersMap.find(elem);
                        sendToClient(opponentPlayer.value().pClientSocket, comVictoryKNB,
                                     id, "Пасс");

                        mainPlayer.value().motionKNB = false;
                        mainPlayer.value().droppedKNB = false;
                        opponentPlayer.value().motionKNB = false;
                        opponentPlayer.value().droppedKNB = false;
                    }
                }
                break;
            }
            // -------------------------------------------------------------------

            // синхронизация времени
            case comToStepKNB:
            {
                int step{};
                in >> step;

                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer.value().countOpponentKNB != 0)
                {
                    for (auto it : mainPlayer.value().idOpponentKNB)
                    {
                        auto elemPlayerOpp = m_playersMap.find(it);
                        sendToClient(elemPlayerOpp.value().pClientSocket, comToStepKNB, step);
                    }
                }
                break;
            }

            // -------------------------------------------------------------------
            // группа
            // хост приглашает слиента в группу
            case comOfferGroup:
            {
                QString idOpp, group;
                in >> idOpp >> group;
                bool ok{};
                int numIdOpp = idOpp.toInt(&ok);

                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer != m_playersMap.end() && ok)
                {
                    auto opponentPlayer = m_playersMap.find(numIdOpp);

                    if (opponentPlayer != m_playersMap.end())
                    {
                        // если true - то клиент состоит в группе
                        if (checkClientGroup(opponentPlayer, group))
                        {
                            sendToClient(mainPlayer.value().pClientSocket, comNotOfferGroup);
                        }
                        else    // иначе клиент не в группе
                        {
                            sendToOfferClient(comOfferGroup, opponentPlayer, id, group);
                        }
                    }
                }
                else
                {
                    sendToClient(mainPlayer.value().pClientSocket, comErrorOfferGroup);
                }
                break;
            }

            // исключить клиента из группы
            case comDelClientGroup:
            {
                QString idOpp, group;
                in >> idOpp >> group;
                bool ok{};
                int numIdOpp = idOpp.toInt(&ok);

                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer != m_playersMap.end() && ok)
                {
                    bool clienGoodDel = false;
                    auto oppPlayer = m_playersMap.find(numIdOpp);

                    if (oppPlayer != m_playersMap.end())
                    {
                        if (group == "group#1" && oppPlayer.value().group_1)
                        {
                            oppPlayer.value().idVecGroup_1.clear();
                            oppPlayer.value().group_1 = false;

                            int index = mainPlayer.value().idVecGroup_1.indexOf(numIdOpp);

                            if (index != -1)
                            {
                                mainPlayer.value().idVecGroup_1.remove(index);

                                // если в группе больше 2 участников
                                if (mainPlayer.value().idVecGroup_1.size() != 0)
                                {
                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDelClientGrouoGood,
                                                 mainPlayer.value().idVecGroup_1, group);

                                    for (auto elem : mainPlayer.value().idVecGroup_1)
                                    {
                                        auto oppPlayer = m_playersMap.find(elem);

                                        // формируем ноый вектор    -------
                                        oppPlayer.value().idVecGroup_1.clear();
                                        oppPlayer.value().idVecGroup_1.push_back(id);
                                        for (auto itId : mainPlayer.value().idVecGroup_1)
                                        {
                                            if (elem != itId)
                                            {
                                                oppPlayer.value().idVecGroup_1.push_back(itId);
                                            }
                                        }
                                        // ----------------

                                        if (oppPlayer != m_playersMap.end())
                                        {
                                            sendToClient(oppPlayer.value().pClientSocket,
                                                         comDelClientGrouoGood,
                                                         oppPlayer.value().idVecGroup_1, group);
                                        }
                                    }
                                }
                                else    // если в группе всего 2 участника
                                {
                                    mainPlayer.value().idVecGroup_1.clear();
                                    mainPlayer.value().group_1 = false;

                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDeleteGroup, -1, group);
                                }
                                clienGoodDel = true;
                            }
                            else
                            {
                                QString strError = "При удалении клиента id: "
                                        + QString::number(id)
                                        + " Из группы: " + group
                                        + " Возникла ошибка: \n Id клиента "
                                          "не найдено в векторе!\n";

                                sendToClient(mainPlayer.value().pClientSocket,
                                             comDelClientGroupError, -1, strError);
                            }
                        }
                        else if (group == "group#2" && oppPlayer.value().group_2)
                        {
                            oppPlayer.value().idVecGroup_2.clear();
                            oppPlayer.value().group_2 = false;

                            int index = mainPlayer.value().idVecGroup_2.indexOf(numIdOpp);

                            if (index != -1)
                            {
                                mainPlayer.value().idVecGroup_2.remove(index);

                                // если в группе больше 2 участников
                                if (mainPlayer.value().idVecGroup_2.size() != 0)
                                {
                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDelClientGrouoGood,
                                                 mainPlayer.value().idVecGroup_2, group);

                                    for (auto elem : mainPlayer.value().idVecGroup_2)
                                    {
                                        auto oppPlayer = m_playersMap.find(elem);

                                        // формируем ноый вектор    -------
                                        oppPlayer.value().idVecGroup_2.clear();
                                        oppPlayer.value().idVecGroup_2.push_back(id);
                                        for (auto itId : mainPlayer.value().idVecGroup_2)
                                        {
                                            if (elem != itId)
                                            {
                                                oppPlayer.value().idVecGroup_2.push_back(itId);
                                            }
                                        }
                                        // ----------------

                                        if (oppPlayer != m_playersMap.end())
                                        {
                                            sendToClient(oppPlayer.value().pClientSocket,
                                                         comDelClientGrouoGood,
                                                         oppPlayer.value().idVecGroup_2, group);
                                        }
                                    }
                                }
                                else    // если в группе всего 2 участника
                                {
                                    mainPlayer.value().idVecGroup_2.clear();
                                    mainPlayer.value().group_2 = false;

                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDeleteGroup, -1, group);
                                }
                                clienGoodDel = true;
                            }
                            else
                            {
                                QString strError = "При удалении клиента id: "
                                        + QString::number(id)
                                        + " Из группы: " + group
                                        + " Возникла ошибка: \n Id клиента "
                                          "не найдено в векторе!\n";

                                sendToClient(mainPlayer.value().pClientSocket,
                                             comDelClientGroupError, -1, strError);
                            }
                        }
                        else if (group == "group#3" && oppPlayer.value().group_3)
                        {
                            oppPlayer.value().idVecGroup_3.clear();
                            oppPlayer.value().group_3 = false;

                            int index = mainPlayer.value().idVecGroup_3.indexOf(numIdOpp);

                            if (index != -1)
                            {
                                mainPlayer.value().idVecGroup_3.remove(index);

                                // если в группе больше 2 участников
                                if (mainPlayer.value().idVecGroup_3.size() != 0)
                                {
                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDelClientGrouoGood,
                                                 mainPlayer.value().idVecGroup_3, group);

                                    for (auto elem : mainPlayer.value().idVecGroup_3)
                                    {
                                        auto oppPlayer = m_playersMap.find(elem);

                                        // формируем ноый вектор    -------
                                        oppPlayer.value().idVecGroup_3.clear();
                                        oppPlayer.value().idVecGroup_3.push_back(id);
                                        for (auto itId : mainPlayer.value().idVecGroup_3)
                                        {
                                            if (elem != itId)
                                            {
                                                oppPlayer.value().idVecGroup_3.push_back(itId);
                                            }
                                        }
                                        // ----------------

                                        if (oppPlayer != m_playersMap.end())
                                        {
                                            sendToClient(oppPlayer.value().pClientSocket,
                                                         comDelClientGrouoGood,
                                                         oppPlayer.value().idVecGroup_3, group);
                                        }
                                    }
                                }
                                else    // если в группе всего 2 участника
                                {
                                    mainPlayer.value().idVecGroup_3.clear();
                                    mainPlayer.value().group_3 = false;

                                    sendToClient(mainPlayer.value().pClientSocket,
                                                 comDeleteGroup, -1, group);
                                }
                                clienGoodDel = true;
                            }
                            else
                            {
                                QString strError = "При удалении клиента id: "
                                        + QString::number(id)
                                        + " Из группы: " + group
                                        + " Возникла ошибка: \n Id клиента "
                                          "не найдено в векторе!\n";

                                sendToClient(mainPlayer.value().pClientSocket,
                                             comDelClientGroupError, -1, strError);
                            }
                        }
                    }

                    if (clienGoodDel)
                    {
                        sendToClient(oppPlayer.value().pClientSocket,comDelClientGroup,
                                     id, group);
                    }
                    else
                    {
                        QString strError = "При удалении клиента id: "
                                + QString::number(numIdOpp)
                                + " Из группы: " + group
                                + " Возникла ошибка: \n Переменная clienGoodDel = false!\n";

                        sendToClient(mainPlayer.value().pClientSocket,
                                     comDelClientGroupError, -1, strError);
                    }
                }
                else
                {
                    QString strError = "При удалении клиента id: "
                            + QString::number(numIdOpp)
                            + " Из группы: " + group
                            + " Возникла ошибка: \n Неудалось преобразовать строку в число!\n";

                    sendToClient(mainPlayer.value().pClientSocket,
                                 comDelClientGroupError, -1, strError);
                }
                break;
            }

            // клиент покидает группу (сам)
            case comLeaveClientGroup:
            {
                QString group;
                in >> group;

                leaveGroups(id, QStringList(group));
                break;
            }

            // расформировать группу
            case comDelTotalGroup:
            {
                QString group;
                in >> group;

                auto mainPlayer = m_playersMap.find(id);

                if (mainPlayer != m_playersMap.end())
                {
                    if (group == "group#1")
                    {
                        for (auto& elem : mainPlayer.value().idVecGroup_1)
                        {
                            auto oppPlayer = m_playersMap.find(elem);
                            oppPlayer.value().idVecGroup_1.clear();
                            oppPlayer.value().group_1 = false;

                            sendToClient(oppPlayer.value().pClientSocket,
                                         comDeleteGroup, -1, group);
                        }
                        mainPlayer.value().idVecGroup_1.clear();
                        mainPlayer.value().group_1 = false;

                        sendToClient(mainPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, group);
                    }
                    else if (group == "group#2")
                    {
                        for (auto& elem : mainPlayer.value().idVecGroup_2)
                        {
                            auto oppPlayer = m_playersMap.find(elem);
                            oppPlayer.value().idVecGroup_2.clear();
                            oppPlayer.value().group_2 = false;

                            sendToClient(oppPlayer.value().pClientSocket,
                                         comDeleteGroup, -1, group);
                        }
                        mainPlayer.value().idVecGroup_2.clear();
                        mainPlayer.value().group_2 = false;

                        sendToClient(mainPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, group);
                    }
                    else if (group == "group#3")
                    {
                        for (auto& elem : mainPlayer.value().idVecGroup_3)
                        {
                            auto oppPlayer = m_playersMap.find(elem);
                            oppPlayer.value().idVecGroup_3.clear();
                            oppPlayer.value().group_3 = false;

                            sendToClient(oppPlayer.value().pClientSocket,
                                         comDeleteGroup, -1, group);
                        }
                        mainPlayer.value().idVecGroup_3.clear();
                        mainPlayer.value().group_3 = false;

                        sendToClient(mainPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, group);
                    }
                }
                break;
            }
            // -------------------------------------------------------------------



        case comTest:
        {
            QVariantList list;
            in >> list;
            for (auto it = list.begin(); it != list.end(); ++it)
            {
                if (it->canConvert(QMetaType::QString))
                    qDebug() << it->toString();
                else
                    qDebug() << it->toInt();
            }
            break;
        }


            default:
            {
                ui->te_info->append("\n Внимание: клиенте: "
                                    + QString::number(id) +
                                    "Прислал неизвесную команду: "
                                    + QString::number(command) + "\n");
                break;
            }
        }
    }
    qDebug() << "-------------------------------- \n";
}



