#include "myserver.h"
#include "ui_myserver.h"



// клиент покинул группу (любую группу)
void MyServer::leaveGroups(int id, QStringList group)
{
    auto mainPlayer = m_playersMap.find(id);
    bool oppGood = true;

    if (group.empty() && mainPlayer != m_playersMap.end())
    {
        group << "group#1" << "group#2" << "group#3" << "KNB";
    }

    for (auto& str : group)
    {
        if (str == "group#1" && mainPlayer.value().group_1)
        {
            for (auto& elem : mainPlayer.value().idVecGroup_1)
            {
                auto oppPlayer = m_playersMap.find(elem);

                int index = oppPlayer.value().idVecGroup_1.indexOf(id);

                if (index != -1)
                {
                    oppPlayer.value().idVecGroup_1.remove(index);

                    // если в группе больше 2 участников
                    if (oppPlayer.value().idVecGroup_1.size() != 0)
                    {
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDelClientGrouoGood,
                                     oppPlayer.value().idVecGroup_1, str);
                    }
                    else    // если в группе всего 2 участника
                    {
                        oppPlayer.value().idVecGroup_1.clear();
                        oppPlayer.value().group_1 = false;

                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, str);
                    }
                }
                else
                {
                    oppGood = false;
                }
            }

            mainPlayer.value().group_1 = false;
            mainPlayer.value().idVecGroup_1.clear();
            sendToClient(mainPlayer.value().pClientSocket, comDeleteGroup, -1, str);
        }
        else if (str == "group#2" && mainPlayer.value().group_2)
        {
            for (auto& elem : mainPlayer.value().idVecGroup_2)
            {
                auto oppPlayer = m_playersMap.find(elem);

                int index = oppPlayer.value().idVecGroup_2.indexOf(id);

                if (index != -1)
                {
                    oppPlayer.value().idVecGroup_2.remove(index);

                    // если в группе больше 2 участников
                    if (oppPlayer.value().idVecGroup_2.size() != 0)
                    {
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDelClientGrouoGood,
                                     oppPlayer.value().idVecGroup_2, str);
                    }
                    else    // если в группе всего 2 участника
                    {
                        oppPlayer.value().idVecGroup_2.clear();
                        oppPlayer.value().group_2 = false;

                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, str);
                    }
                }
                else
                {
                    oppGood = false;
                }
            }

            mainPlayer.value().group_2 = false;
            mainPlayer.value().idVecGroup_2.clear();
            sendToClient(mainPlayer.value().pClientSocket, comDeleteGroup, -1, str);
        }
        else if (str == "group#3" && mainPlayer.value().group_3)
        {
            for (auto& elem : mainPlayer.value().idVecGroup_3)
            {
                auto oppPlayer = m_playersMap.find(elem);

                int index = oppPlayer.value().idVecGroup_3.indexOf(id);

                if (index != -1)
                {
                    oppPlayer.value().idVecGroup_3.remove(index);

                    // если в группе больше 2 участников
                    if (oppPlayer.value().idVecGroup_3.size() != 0)
                    {
                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDelClientGrouoGood,
                                     oppPlayer.value().idVecGroup_3, str);
                    }
                    else    // если в группе всего 2 участника
                    {
                        oppPlayer.value().idVecGroup_3.clear();
                        oppPlayer.value().group_3 = false;

                        sendToClient(oppPlayer.value().pClientSocket,
                                     comDeleteGroup, -1, str);
                    }
                }
                else
                {
                    oppGood = false;
                }
            }

            mainPlayer.value().group_3 = false;
            mainPlayer.value().idVecGroup_3.clear();
            sendToClient(mainPlayer.value().pClientSocket, comDeleteGroup, -1, str);
        }
        else if (str == "KNB")
        {
            leaveGroupsKNB(id);
        }

        if (!oppGood)
        {
            QString strError = "При удалении клиента id: "
                    + QString::number(id)
                    + " Из группы: " + str
                    + " Возникла ошибка: \n Id клиента "
                      "не найдено в векторе!\n";

            sendToClient(mainPlayer.value().pClientSocket,
                         comDelClientGroupError, -1, strError);
        }
    }
}


// клиент покинул группу KNB (вышел)
void MyServer::leaveGroupsKNB(int id)
{
    auto mainPlayer = m_playersMap.find(id);

    if (mainPlayer != m_playersMap.end())
    {
        mainPlayer.value().gameKNB = false;
        mainPlayer.value().countOpponentKNB = 0;

        if (mainPlayer.value().idOpponentKNB.size() == 1)
        {
            auto opponentPlayer = m_playersMap.find(mainPlayer.value().idOpponentKNB.at(0));
            opponentPlayer.value().gameKNB = false;
            opponentPlayer.value().countOpponentKNB = 0;
            opponentPlayer.value().idOpponentKNB.clear();
            opponentPlayer.value().idMotionKNB.clear();
            opponentPlayer.value().answerKNB.clear();
            opponentPlayer.value().motionKNB = false;
            opponentPlayer.value().droppedKNB = false;
            opponentPlayer.value().victoryPoints = 0;
            opponentPlayer.value().nextKNB = false;
            sendToClient(opponentPlayer.value().pClientSocket, comGroupDisbandedKNB);
        }
        else
        {
            for (auto itId : mainPlayer.value().idOpponentKNB)
            {
                auto opponentPlayer = m_playersMap.find(itId);
                sendToClient(opponentPlayer.value().pClientSocket,
                             comClientLeaveGroupsKNB, id);

                int index = opponentPlayer.value().idOpponentKNB.indexOf(id);

                if (index != -1)
                {
                    opponentPlayer.value().idOpponentKNB.remove(index);
                }
            }
        }

        mainPlayer.value().idOpponentKNB.clear();
        mainPlayer.value().gameKNB = false;
        mainPlayer.value().countOpponentKNB = 0;
        mainPlayer.value().idOpponentKNB.clear();
        mainPlayer.value().idMotionKNB.clear();
        mainPlayer.value().answerKNB.clear();
        mainPlayer.value().motionKNB = false;
        mainPlayer.value().droppedKNB = false;
        mainPlayer.value().victoryPoints = 0;
        mainPlayer.value().nextKNB = false;
    }
}




// ----------------------------------------------------------------------
// проверка состоит ли клиент в группе
bool MyServer::checkClientGroup(QMap<int, Player>::iterator opponentPlayer, QString &group)
{
    if (group == "group#1")
    {
        return opponentPlayer.value().group_1;
    }
    else if (group == "group#2")
    {
        return opponentPlayer.value().group_2;
    }
    else if (group == "group#3")
    {
        return opponentPlayer.value().group_3;
    }
    else if (group == "KNB")
    {
        return opponentPlayer.value().gameKNB;
    }
    return false;
}
// ----------------------------------------------------------------------


// отправка сообщения для группы
void MyServer::sendMessageGroup(QVector<int> &vec, QString &strMessage, int id)
{
    for (auto& elem : vec)
    {
        auto oppPlayer = m_playersMap.find(elem);
        sendToClient(oppPlayer.value().pClientSocket, comMessagePullGroup,
                     id, strMessage);
    }
}
