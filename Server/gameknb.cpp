#include "myserver.h"
#include "ui_myserver.h"



/*
 * Правила игры KNB:
 *
 * если игроков 2 то просто сравниваются ответы
 *
 * если игроков больше двух то игра идёт на очки
 * то есть сколько игрок победил противников столько у него и очков
 *
 * пример на 6 участников:
 *
 * иг1 - камень
 * иг2 - ножницы
 * иг3 - бумага
 * иг4 - камень
 * иг5 - бумага
 * иг6 - камень
 *
 * игроки №1, №4, №6 - выбрали камень и смогли победить игрока №2, так как у него ножницы
 * следовательно каждый набрал по 1 очку каждый
 *
 * игрока №2 - выбрал ножницы и победил игроки №3 и №5, так как у них бумага
 * следовательно заработал 2 очка
 *
 * игроки №3 и №5 смогли победить игроков №1, №4, №6, так как у них камни
 * следовательно заработали 3 очка каждый
 *
 * А значит игроки №3 и №5 победили и состязаться между собой
 * остальные выбывают
 *
 */


/*!
 * \brief responseCode - получить код ответа
 * \param answerOne - ответ первого игрока, answerTwo - ответ втрого игрока
 *
 * 1 - победил первый
 * 2 - победил втрой
 * 3 - ничья
 */
int MyServer::responseCode(QString answerOne, QString answerTwo)
{
    if (answerOne == answerTwo)
    {
        return 3;
    }

    if (answerOne == "")
    {
        return 2;
    }
    else if (answerTwo == "")
    {
        return 1;
    }

    if (answerOne == "Камень" && answerTwo == "Ножницы")
    {
        return 1;
    }
    else if (answerOne == "Камень" && answerTwo == "Бумага")
    {
        return 2;
    }
    else if (answerOne == "Ножницы" && answerTwo == "Камень")
    {
        return 2;
    }
    else if (answerOne == "Ножницы" && answerTwo == "Бумага")
    {
        return 1;
    }
    else if (answerOne == "Бумага" && answerTwo == "Камень")
    {
        return 1;
    }
    else if (answerOne == "Бумага" && answerTwo == "Ножницы")
    {
        return 2;
    }
}


void MyServer::victoryPointsPlayer(QMap<int, Player>::iterator itOne,
                                   QMap<int, Player>::iterator itTwo)
{
    int answer = 0;
    answer = responseCode(itOne.value().answerKNB,
                 itTwo.value().answerKNB);

    switch (answer)
    {
        case 0:
        {
            qDebug() << "\n\n !! ERROR !!!! \n\n";
            break;
        }

        case 1:     // победил первый
        {
            itOne.value().victoryPoints += 1;
            break;
        }

        case 2:     // победил втрой
        {
            itTwo.value().victoryPoints += 1;
            break;
        }

        case 3:     // ничья
        {
            //
            break;
        }
    }

    itOne.value().idMotionKNB.push_back(itTwo.value().id);

    ui->te_info->append("\n  player:  "
                        + QString::number(itOne.value().id)
                        + "  vic po:  " +
                QString::number(itOne.value().victoryPoints));

    ui->te_info->append("\n  player:  "
                        + QString::number(itTwo.value().id)
                        + "  vic po:  " +
                        QString::number(itTwo.value().victoryPoints));
}


// подчет очков и определение победителя (или старт 2 раунда)
void MyServer::motionKNB(QMap<int, Player>::iterator mainPlayer)
{
    qDebug() << "\n ----------------------------- \n"
             << Q_FUNC_INFO;

    // подсчёт очков
    for (auto elem : mainPlayer.value().idOpponentKNB)
    {
        ui->te_info->append("\n mainPlayer = " +
                            QString::number(mainPlayer.value().id));

        auto opponentPlayer = m_playersMap.find(elem);

        ui->te_info->append("opponentPlayer = " +
                            QString::number(opponentPlayer.value().id));

        //
        if (!opponentPlayer.value().droppedKNB)
        {
            for (auto oppElem : opponentPlayer.value().idOpponentKNB)
            {
                auto oppPlayerTwo = m_playersMap.find(oppElem);

                ui->te_info->append("oppPlayerTwo = " +
                                    QString::number(oppPlayerTwo.value().id));

                if (oppPlayerTwo.value().id != mainPlayer.value().id
                        && !oppPlayerTwo.value().droppedKNB
                        && !opponentPlayer.value().droppedKNB
                        )
                {
                    int index = oppPlayerTwo.value().idMotionKNB.indexOf(opponentPlayer.value().id);

                    if(index == -1)
                    {
                        victoryPointsPlayer(opponentPlayer, oppPlayerTwo);
                    }
                    else
                    {
                        opponentPlayer.value().idMotionKNB.push_back(oppPlayerTwo.value().id);
                    }
                }
            }
        }
        else
        {
            opponentPlayer.value().victoryPoints = -1;
        }

        // если клиент не выбыл
        if (!mainPlayer.value().droppedKNB
                && !opponentPlayer.value().droppedKNB)
        {
            victoryPointsPlayer(mainPlayer, opponentPlayer);
            mainPlayer.value().idMotionKNB.push_back(opponentPlayer.value().id);
        }
    }

    // определение победителя
    int idPlayer = mainPlayer.value().id;
    int points = mainPlayer.value().victoryPoints;

    QVector<int> vec;
    vec.push_back(idPlayer);

    for (auto elem : mainPlayer.value().idOpponentKNB)
    {
        auto opponentPlayer = m_playersMap.find(elem);

        if (points < opponentPlayer.value().victoryPoints)
        {
            idPlayer = opponentPlayer.value().id;
            points = opponentPlayer.value().victoryPoints;
            vec.clear();
            vec.push_back(idPlayer);
        }
        else if (points == opponentPlayer.value().victoryPoints)
        {
            vec.push_back(opponentPlayer.value().id);
        }
    }

    qDebug() << "vec size: " << vec.size();
    ui->te_info->append("vec size: " + QString::number(vec.size()));

    // если количество победителей равно колучеству участников  -  значит ничья
    if (vec.size() == (mainPlayer.value().idOpponentKNB.size() + 1)
            || vec.size() == mainPlayer.value().idOppRaundNextKNB.size())
    {
        ui->te_info->append("\n Ничья!");
        for (auto elem : mainPlayer.value().idOpponentKNB)
        {
            auto playerDrav = m_playersMap.find(elem);

            ui->te_info->append("playerDrav  id = " + QString::number(playerDrav.value().id));
            ui->te_info->append("droppedKNB = " + QString::number(playerDrav.value().droppedKNB));

            if (!playerDrav.value().droppedKNB)
            {
                sendToClient(playerDrav.value().pClientSocket, comDrawKNB);
                clearDataPlayer(playerDrav);
            }
            else
            {
                sendToClient(playerDrav.value().pClientSocket, comNextPlaYKNB);
                clearDataPlayer(playerDrav);
            }
        }

        // очистка мейна
        if (!mainPlayer.value().droppedKNB)
        {
            sendToClient(mainPlayer.value().pClientSocket, comDrawKNB);
            clearDataPlayer(mainPlayer);
        }
        else
        {
            sendToClient(mainPlayer.value().pClientSocket, comNextPlaYKNB);
            clearDataPlayer(mainPlayer);
        }
    }
    else
    {
        // следующий раунд
        if (vec.size() > 1)
        {
            // отправляем команду всем прошедшим в следующий раунд
            for (auto elem : vec)
            {
                qDebug() << "Client victoty: " << elem;
                ui->te_info->append("Client victoty: " + QString::number(elem));

                auto playerVictory = m_playersMap.find(elem);
                sendToClient(playerVictory.value().pClientSocket, comNextKNB, vec);
                playerVictory.value().nextKNB = true;
            }

            // ------------- выбывшие игроки
            if (!mainPlayer.value().nextKNB)
            {
                sendToClient(mainPlayer.value().pClientSocket, comDefeatNextKNB, vec);
                clearDataPlayer(mainPlayer, true);
            }

            for (auto elem : mainPlayer.value().idOpponentKNB)
            {
                qDebug() << "opponent player: " << elem;
                ui->te_info->append("opponent player: " + QString::number(elem));

                auto opponentPlayer = m_playersMap.find(elem);

                if (!opponentPlayer.value().nextKNB)
                {
                    sendToClient(opponentPlayer.value().pClientSocket, comDefeatNextKNB, vec);
                    clearDataPlayer(opponentPlayer, true);
                }
            }
            // -----------------------

            // очистка данных победителей
            for (auto elem : vec)
            {
                auto playerVictory = m_playersMap.find(elem);
                clearDataPlayer(playerVictory);

                // запись в вектор id всех победителей
                playerVictory.value().idOppRaundNextKNB = vec;
            }
        }
        else    // один победитель
        {
            auto playerVictory = m_playersMap.find(vec.at(0));

            QVector<int> vecId;
            QString strAnswer;

            for (auto elem : playerVictory.value().idOpponentKNB)
            {
                auto opponentPlayer = m_playersMap.find(elem);
                sendToClient(opponentPlayer.value().pClientSocket, comDefeatKNB, vec.at(0),
                             playerVictory.value().answerKNB);

                vecId.push_back(opponentPlayer.value().id);

                if (opponentPlayer.value().answerKNB == "")
                {
                    strAnswer += "Пасс, ";
                }
                else
                {
                    strAnswer += opponentPlayer.value().answerKNB + ", ";
                }

                clearDataPlayer(opponentPlayer);
            }

            ui->te_info->append("vecId = " + QString::number(vecId.size()));

            // отправка данных победителю
            sendToClient(playerVictory.value().pClientSocket, comVictoryKNB, vecId, strAnswer);
            clearDataPlayer(playerVictory);
        }
    }


    // вывод данных
    ui->te_info->append("\n\n ---------------------------------------\n");

    ui->te_info->append("main player: " + QString::number(mainPlayer.value().id));
    ui->te_info->append(QString::number(mainPlayer.value().motionKNB));
    ui->te_info->append(QString::number(mainPlayer.value().droppedKNB));
    ui->te_info->append("next = " + QString::number(mainPlayer.value().nextKNB));
    ui->te_info->append("vec = " + QString::number(!mainPlayer.value().idMotionKNB.isEmpty()));
    ui->te_info->append(QString::number(mainPlayer.value().victoryPoints));

    for (auto elem : mainPlayer.value().idOpponentKNB)
    {
        auto opponentPlayer = m_playersMap.find(elem);
        ui->te_info->append("opponent Player: " + QString::number(opponentPlayer.value().id));
        ui->te_info->append(QString::number(opponentPlayer.value().motionKNB));
        ui->te_info->append(QString::number(opponentPlayer.value().droppedKNB));
        ui->te_info->append("next = " + QString::number(opponentPlayer.value().nextKNB));
        ui->te_info->append("vec = "
                            + QString::number(!opponentPlayer.value().idMotionKNB.isEmpty()));
        ui->te_info->append(QString::number(opponentPlayer.value().victoryPoints));
    }
    ui->te_info->append("\n ------------------  end  ---------------------\n\n");


    // сброс параметров играков (для тестов)
//    for (auto elem : mainPlayer.value().idOpponentKNB)
//    {
//        auto opponentPlayer = m_playersMap.find(elem);
//        opponentPlayer.value().motionKNB = false;
//        opponentPlayer.value().droppedKNB = false;
//        opponentPlayer.value().victoryPoints = 0;
//    }
//    mainPlayer.value().motionKNB = false;
//    mainPlayer.value().droppedKNB = false;
//    mainPlayer.value().victoryPoints = 0;

    qDebug() << "-------------------------------- \n";
}

// подчет очков и определение ЕСЛИ 2 участника !!!
void MyServer::response(QMap<int, Player>::iterator mainPlayer)
{
    auto oppPla = m_playersMap.find(mainPlayer.value().idOpponentKNB.at(0));
    int answer = responseCode(mainPlayer.value().answerKNB,
                 oppPla.value().answerKNB);

    ui->te_info->append("\n answer = " +
                        QString::number(answer) + "\n");

    switch (answer)
    {
        case 1:     // победил первый
        {
            sendToClient(mainPlayer.value().pClientSocket, comVictoryKNB,
                         oppPla.value().id, oppPla.value().answerKNB);
            sendToClient(oppPla.value().pClientSocket, comDefeatKNB,
                         mainPlayer.value().id, mainPlayer.value().answerKNB);
            break;
        }

        case 2:     // победил втрой
        {
            sendToClient(oppPla.value().pClientSocket, comVictoryKNB,
                         mainPlayer.value().id, mainPlayer.value().answerKNB);
            sendToClient(mainPlayer.value().pClientSocket, comDefeatKNB,
                         oppPla.value().id, oppPla.value().answerKNB);
            break;
        }

        case 3:     // ничья
        {
            sendToClient(oppPla.value().pClientSocket, comDrawKNB);
            sendToClient(mainPlayer.value().pClientSocket, comDrawKNB);
            break;
        }
    }

    mainPlayer.value().motionKNB = false;
    mainPlayer.value().droppedKNB = false;
    oppPla.value().motionKNB = false;
    oppPla.value().droppedKNB = false;
}

void MyServer::clearDataPlayer(QMap<int, Player>::iterator mainPlayer, bool drop)
{
    mainPlayer.value().motionKNB = drop;
    mainPlayer.value().droppedKNB = drop;
    mainPlayer.value().nextKNB = false;
    mainPlayer.value().victoryPoints = 0;
    mainPlayer.value().idMotionKNB.clear();
    mainPlayer.value().idOppRaundNextKNB.clear();
    mainPlayer.value().answerKNB.clear();
}






