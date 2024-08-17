#include "myclient.h"
#include "ui_myclient.h"



void MyClient::slotReabyRead()      // команнда пришла с сервера
{
    debugToInfo("Вызван метод: slotReabyRead");

    QDataStream in(m_pTcpSocket);
    in.setVersion(QDataStream::Qt_5_11);

    while(true)
    {
        if (!m_nNextBlockSize)
        {
            if (m_pTcpSocket->bytesAvailable() < sizeof(quint16))
            {
                break;
            }
            in >> m_nNextBlockSize;
        }

        if (m_pTcpSocket->bytesAvailable() < m_nNextBlockSize)
        {
            debugToInfo("ОШИБКА!!! Пришел некоректный пакет данных!!!"
                        + QString::number(m_pTcpSocket->bytesAvailable())
                        + " " + QString::number(m_nNextBlockSize), 1);

            ui->gb_errorInfo->setVisible(true);
            ui->lb_errorText->setText("Внимание!!! \n"
                                      "Пришел некоректный пакет данных! \n"
                                      "Нажмите на кнопку \"break\" что бы обновить информацию. ");

            break;
        }

        m_nNextBlockSize = 0;

        Tranciver_commmand command;
        in >> command;

        debugToInfo("Пришла команда: " + QString::number(command));

        switch (command)
        {
            case comRegistratiomYes:
            {
                in >> idClient;

                systemChatInfo("Клиент зарегистрировался на сервере \n Ваш id:  "
                               + QString::number(idClient));
                ui->lb_myId->setText(QString::number(idClient));

                // разблокирую вкладки с играми
                enabledGameForm();
                enabledFieldForm(false);    // блокировка полей регестрации при подключении
                statusChatPrivate();    // статус чата личных сообщений
                ui->cb_offline->setEnabled(false);
                ui->tab_listClientAndGroup->setTabEnabled(1, true);    // вкладка группы
                ui->b_nameOfferClientGroup->setEnabled(true);
                ui->pb_log_in->setText("Отключиться от сервера");
                chatBlok();     // разблокировка чата
                ui->gb_mesegChat->setEnabled(!m_privatWinChat); // если при старте выбран личный чат
                onlineFlag = true;
                ++m_blokConnectServer;  // для зашиты от спама
                break;
            }

            case comErrorCreationID:
            {
                debugToInfo("Возникла ошибка при регестрации на сервере", 2);
                systemChatInfo("Возникла ошибка при регестрации на сервере \n Попробуйте позже");

                ui->lb_myId->setText("offline");
                enabledFieldForm();
                break;
            }

            case comNotDataBase:
            {
                QMessageBox::information(this, "Нет базы данных",
                                         "На сервере не установленна база данных",
                                         QMessageBox::Close,
                                         QMessageBox::Close);
                break;
            }

            // ----------------------------------------------
            // KNB
            case comOfferToJoinKNB:
            {
                debugToInfo("Приглашение на партию");

                int idOpp{};
                QString game;
                in >> idOpp >> game;

                offerToJoin(idOpp, game);

//                if (idOpponent_1 == -1)
//                {
//                    in >> idOpponent_1 >> gameOffer_1;

//                    ui->gb_game_1->setVisible(true);
//                    ui->lb_box_1->setText("Id: " + QString::number(idOpponent_1)
//                                          + " пригласил на партию: \n" + gameOffer_1);
//                }
//                else if (idOpponent_2 == -1)
//                {
//                    in >> idOpponent_2 >> gameOffer_2;
//                    ui->gb_game_2->setVisible(true);
//                    ui->lb_box_2->setText("Id: " + QString::number(idOpponent_2)
//                                          + " пригласил на партию: \nКамень ножницы бумага");
//                }
//                else if (idOpponent_3 == -1)
//                {
//                    in >> idOpponent_3 >> gameOffer_3;
//                    ui->gb_game_3->setVisible(true);
//                    ui->lb_box_3->setText("Id: " + QString::number(idOpponent_3)
//                                          + " пригласил на партию: \nКамень ножницы бумага");
//                }
//                else
//                {
//                    //
//                }

                debugToInfo("Переменная idOpponent_1 = " + QString::number(idOpponent_1));
                debugToInfo("Переменная gameOffer_1 = " + gameOffer_1);
                debugToInfo("Переменная idOpponent_2 = " + QString::number(idOpponent_2));
                debugToInfo("Переменная gameOffer_2 = " + gameOffer_2);
                debugToInfo("Переменная idOpponent_3 = " + QString::number(idOpponent_3));
                debugToInfo("Переменная gameOffer_3 = " + gameOffer_3);

                break;
            }

            // закрытие приглашений
            case comCloseInvitation:
            {
                int oppId;
                QString strGame;
                in >> oppId >> strGame;

                debugToInfo("Сокрытие приглашений");
                debugToInfo("Пришли данные:");
                debugToInfo("Переменная oppId = " + QString::number(oppId));
                debugToInfo("Переменная strGame = " + strGame);

                if (idOpponent_1 == oppId && gameOffer_1 == strGame)
                {
                    ui->gb_game_1->setVisible(false);
                    debugToInfo("приглашение 1 скрыто");
                    idOpponent_1 = -1;
                    gameOffer_1.clear();
                }
                if (idOpponent_2 == oppId && gameOffer_2 == strGame)
                {
                    ui->gb_game_2->setVisible(false);
                    debugToInfo("приглашение 2 скрыто");
                    idOpponent_2 = -1;
                    gameOffer_2.clear();
                }
                if (idOpponent_3 == oppId && gameOffer_3 == strGame)
                {
                    ui->gb_game_3->setVisible(false);
                    debugToInfo("приглашение 3 скрыто");
                    idOpponent_3 = -1;
                    gameOffer_3.clear();
                }
                break;
            }

            // клиент принял приглашение
            case comOpponentOffer:      // пришел список (вектор) всех участников группы
            {                           // тут нет ошибок (по приходу данных)
                QString strGame;
                in >> strGame;

                debugToInfo("Принето приглашение на: " + strGame);

                // здесь записываються все группы (в том числе и KNB - это надо сделать!!)
                // очистка данных для последующей записи
                if (!m_group_1 && !m_group_2 && !m_group_3 && !m_groupKNB)
                {
                    ui->lb_names_groups->clear();
                }

                if (strGame == "KNB")
                {
                    vecOpponentKNB.clear();
                    in >> vecOpponentKNB; //>> m_idHost_KNB;

                    // проверка на хост
//                    if (m_idHost_KNB <= 0)
//                    {
//                        m_hostGroup_KNB = true;
//                    }

                    debugToInfo("Пришел вектор размером: "
                                + QString::number(vecOpponentKNB.size()));

                    ui->lb_myOpponent->clear();

                    if (!m_groupKNB)
                    {
                        ui->lb_id_groups_KNB->clear();
                        ui->lb_names_groups->setText(ui->lb_names_groups->text() + "  KNB,");
                        // увеличиваем количество групп в которых состоит клиент
                        ++m_countGroup;
                        textNameChatGroup();
                    }
                    m_groupKNB = true;

                    textClientLabelGroup(ui->lb_id_groups_KNB, vecOpponentKNB,
                                         m_idHost_KNB, m_hostGroup_KNB);

                    QString strOpp;
                    strOpp = "Противник для игры найден! \n"
                             + QString(vecOpponentKNB.size() > 1 ? "Ваши противники: "
                                                                 : "Ваш противник: ");

                    for (auto it : vecOpponentKNB)
                    {
                        strOpp += QString::number(it) + ", ";
                        ui->lb_myOpponent->setText(ui->lb_myOpponent->text()
                                                   + "  " + QString::number(it));
                    }
                    systemChatInfo(strOpp);

                    closeGameOffer("KNB");

                    chekClientGroup();

                    // синхронизация времени
                    if (vecOpponentKNB.size() + 1 < ui->sb_countPlayerKNB->value())
                    {
                        debugToInfo("Cинхронизация времени");
                        sendToServer(comToStepKNB);
                    }

                    // блокировка настроек игры
                    ui->hs_countPlayerKNB->setEnabled(false);
                    ui->sb_countPlayerKNB->setEnabled(false);
                }
                else if (strGame == "group#1")  // группа 1
                {
                    vecGropu_1.clear();
                    in >> vecGropu_1 >> m_idHost_1;

                    debugToInfo("Флаг хоста: " + QString::number(m_idHost_1));

                    // проверка на хост
                    if (m_idHost_1 <= 0)
                    {
                        m_hostGroup_1 = true;
                    }

                    debugToInfo("Пришел вектор размером: "
                                + QString::number(vecGropu_1.size()));

                    systemChatInfo(QString(m_hostGroup_1 ? "Клиент принял приглашение"
                                                         : "Вы приняли приглашение"));

                    if (!m_group_1)
                    {
                        ui->lb_id_groups->clear();
                        ui->lb_names_groups->setText(ui->lb_names_groups->text() + "  group#1,");
                        // увеличиваем количество групп в которых состоит клиент
                        ++m_countGroup;
                        textNameChatGroup();
                    }
                    m_group_1 = true;

                    textClientLabelGroup(ui->lb_id_groups, vecGropu_1, m_idHost_1, m_hostGroup_1);

                    // убрать одинаковые приглашения
                    closeGameOffer("group#1");

                    chekClientGroup();
                }
                else if (strGame == "group#2")  // группа 2
                {
                    vecGropu_2.clear();
                    in >> vecGropu_2 >> m_idHost_2;

                    debugToInfo("Флаг хоста: " + QString::number(m_idHost_2));

                    // проверка на хост
                    if (m_idHost_2 <= 0)
                    {
                        m_hostGroup_2 = true;
                    }

                    debugToInfo("Пришел вектор размером: "
                                + QString::number(vecGropu_2.size()));

                    systemChatInfo(QString(m_hostGroup_2 ? "Клиент принял приглашение"
                                                         : "Вы приняли приглашение"));

                    if (!m_group_2)
                    {
                        ui->lb_id_groups_2->clear();
                        ui->lb_names_groups->setText(ui->lb_names_groups->text() + "  group#2,");
                        // увеличиваем количество групп в которых состоит клиент
                        ++m_countGroup;
                        textNameChatGroup();
                    }
                    m_group_2 = true;

                    textClientLabelGroup(ui->lb_id_groups_2, vecGropu_2, m_idHost_2, m_hostGroup_2);

                    // убрать одинаковые приглашения
                    closeGameOffer("group#2");

                    chekClientGroup();
                }
                else if (strGame == "group#3")  // группа 3
                {
                    vecGropu_3.clear();
                    in >> vecGropu_3 >> m_idHost_3;

                    debugToInfo("Флаг хоста: " + QString::number(m_idHost_3));

                    // проверка на хост
                    if (m_idHost_3 <= 0)
                    {
                        m_hostGroup_3 = true;
                    }

                    debugToInfo("Пришел вектор размером: "
                                + QString::number(vecGropu_3.size()));

                    systemChatInfo(QString(m_hostGroup_3 ? "Клиент принял приглашение"
                                                         : "Вы приняли приглашение"));

                    if (!m_group_3)
                    {
                        ui->lb_id_groups_3->clear();
                        ui->lb_names_groups->setText(ui->lb_names_groups->text() + "  group#3,");
                        // увеличиваем количество групп в которых состоит клиент
                        ++m_countGroup;
                        textNameChatGroup();
                    }
                    m_group_3 = true;

                    textClientLabelGroup(ui->lb_id_groups_3, vecGropu_3, m_idHost_3, m_hostGroup_3);

                    // убрать одинаковые приглашения
                    closeGameOffer("group#3");

                    chekClientGroup();
                }
                break;
            }

            case comGroupAssembledKNB:
            {
                systemChatInfo("Группа собрана");
                visibleGameKNB();
                stopTimerFindGroups("Группа собрана");
                break;
            }

            case comGroupDisbandedKNB:      // группа расформирована
            {
                systemChatInfo("Группа расформирована");
                ui->lb_myOpponent->clear();
                ui->lb_statusFindGrup->setText("Группа расформирована");
                visibleGameKNB(false);

                delClientGroupKNB();
//                vecOpponentKNB.clear();
//                m_idHost_KNB = 0;
//                m_groupKNB = false;
//                m_hostGroup_KNB = false;
//                ui->lb_myOpponent->setText("Нет данных");
//                ui->lb_id_groups_KNB->setText("Нет данных");
//                setTextLabelGroup("KNB");
                questionFormKNBDefolt();
                enabledGameKNB(true);

                if (m_timerFormKNBStrart)       // если таймер запущен
                {
                    stopTimerFindGroupsAndMotionKNB();
                    stopTimerFindGroups();
                    blocFormButtonKNB(true);
                }
                infoMotionFormKNB("");

                break;
            }



            // повторный запуск таймера при остановки
            // пришла команда, что клиент продолжает играть
            // и если у этого клиента висит вопрос, то просто информируется
            // иначе запускается таймер и отправляется всем клентам команда на запус таймера
            case comTimerMotionStartStopKNB:
            {
                int oppId{};
                in >> oppId;

                if (!questionFormKNBVisible)
                {
                    sendToServer(comTimerMotionKNB);
                }
                else
                {
                    // временно
                    // придумать как показывать всех играков в игре
                    infoMotionFormKNB("Игрок: " + QString::number(oppId)
                                      + " продолжил игру.");
                }
                break;
            }
            // здесь, все клинты согласились играть дальше и таймер запускается
            case comTimerMotionKNB:
            {
                timerFindGroupsAndMotionKNB.start();
                break;
            }

            case comClientLeaveGroupsKNB:
            {
                int idLeave;
                in >> idLeave;

                // если сделан ход то запушен таймер - останавливаю его
                if (m_motionKNB || m_motionOppjnentKNB)
                {
                    timerFindGroupsAndMotionKNB.stop();
                }

                debugToInfo("Клиент покинул группу: " + QString::number(idLeave));
                debugToInfo("Окно вопроса открыто: " + QString::number(questionFormKNBVisible));

                int index = vecOpponentKNB.indexOf(idLeave);
                if (index != -1)
                {
                    vecOpponentKNB.remove(index);
                    ui->lb_myOpponent->clear();
                    for (auto it : vecOpponentKNB)
                    {
                        ui->lb_myOpponent->setText(ui->lb_myOpponent->text()
                                                   + "  " + QString::number(it));
                    }
                }

                textClientLabelGroup(ui->lb_id_groups_KNB, vecOpponentKNB,
                                     m_idHost_KNB, m_hostGroup_KNB);

                if (!questionFormKNBVisible)
                {
                    questionFormKNB("Клиент: " + QString::number(idLeave)
                                    + " покинул группу. Продолжить игру?",
                                    "Продолжить игру", "Покинуть группу");
                    m_statusFormKNB = clientLeaveKNB;
                }
                else
                {
                    QString text = ui->lb_questionFormKNB->text();
                    QStringList list;

                    debugToInfo("text: " + text);

                    QRegularExpression re("\\d+");        // "\\d+"       "\\b\\d+\\b"
                    QRegularExpressionMatchIterator it = re.globalMatch(text);
                    while (it.hasNext())
                    {
                        QRegularExpressionMatch match = it.next();
                        list << match.captured(0);
                        debugToInfo(match.captured(0));
                    }
                    QString temp = list.join(" ");
                    ui->lb_questionFormKNB->setText("Клиенты: "
                                                   + temp + " "
                                                   + QString::number(idLeave)
                                                   + " покинули группу. Продолжить игру?");
                    debugToInfo(temp);
                }

                visibleGameKNB(false);
                ui->pb_gameStartKNB->setVisible(false);
                break;
            }


            // --------     опонент сделал ход
            case comMotionStartTimerKNB:
            {
                int idOpp{};
                in >> idOpp;

                infoMotionFormKNB("Игрок: " + QString::number(idOpp) + " сделал ход!");

                // если какойто из участников игры первым сделал ход
                // то таймер запускается только 1 раз
                if (!m_motionOppjnentKNB)
                {
                    timerFindGroupsAndMotionKNB.start();
                    m_motionOppjnentKNB = true;

                    debugToInfo("Таймер ожидания хода запущен");
                }
                break;
            }

            // --------     победа
            case comVictoryKNB:
            {
                int oppId{};
                QString strAnswer;
                QVector<int> vecId;

                if (vecOpponentKNB.size() > 1)
                {
                    in >>strAnswer >> vecId ;

                    debugToInfo("пришел вектор проигравших: " + QString::number(vecId.size()));

                    QString strOppId;
                    for (auto& elem: vecId)
                    {
                        strOppId += QString::number(elem) + ", ";
                    }

                    infoMotionFormKNB("Вы победили!!!! \n Проиграл: " + strOppId
                                      + "\n Они выбрали: " + strAnswer);
                }
                else
                {
                    in >> oppId >> strAnswer;
                    infoMotionFormKNB("Вы победили!!!! \n Проиграл: " + QString::number(oppId)
                                      + "\n Он выбрал: " + strAnswer);
                }

                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                blocFormButtonKNB(true);
                break;
            }

            // --------     поражение
            case comDefeatKNB:
            {
                int oppId{};
                QString strAnswer;
                in >> oppId >> strAnswer;

                infoMotionFormKNB("Вы проиграли  :( \n Победил: " + QString::number(oppId)
                                  + "\n Он выбрал: " + strAnswer);
                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                blocFormButtonKNB(true);
                break;
            }

            // ---------    ничья
            case comDrawKNB:
            {
                infoMotionFormKNB("Ничья  :(");
                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                blocFormButtonKNB(true);
                break;
            }

            // ----------    следующий раунд
            case comNextKNB:
            {
                QVector<int> vec;
                in >> vec;

                debugToInfo("следующий раунд", 3);
                debugToInfo("vec size: " + QString::number(vec.size()), 3);

                infoMotionFormKNB("Вы прошли в следующий раунд \n Ваши противники: ", vec);
                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                blocFormButtonKNB(true);
                break;
            }

            // -----------    пропуск рaунда
            case comDefeatNextKNB:
            {
                QVector<int> vec;
                in >> vec;

                debugToInfo("пропуск рaунда", 3);
                debugToInfo("vec size: " + QString::number(vec.size()), 3);

                infoMotionFormKNB("Вы проиграли  :( \n В следующий раудн прошли: ", vec);
                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                break;
            }

            // -----------    продолжение игры после пропуска раунда
            case comNextPlaYKNB:
            {
                debugToInfo("Игра продолжается", 3);
                systemChatInfo("Игра продолжается");
                infoMotionFormKNB("Игра продолжается");

                stopTimerFindGroupsAndMotionKNB();
                stopTimerFindGroups();
                blocFormButtonKNB(true);
                break;
            }
            // ----------------------------------------------

            case comToStepKNB:     // количество времени для поиска
            {
                timerFindGroupsAndMotionKNB.stop();
                enabledGameKNB(false);
                m_step = 0;
                m_stepOpposite = 60;
                in >> m_step;
                m_stepOpposite -= m_step;
                m_clientRoot = false;
                timerFindGroupsAndMotionKNB.start();
                break;
            }

            case comStopFindGroupsKNB:      // хост остановил таймер поиска
            {
                stopTimerFindGroupsAndMotionKNB();
                break;
            }







            // -----------------------------------------------
            // это надо удалить
            case comPairFound:      //
            {
                int id{};
                in >> id;

                systemChatInfo("Противник для игры найден! \n"
                               "Ваш противник: " + QString::number(id));
                ui->lb_myOpponent->setText(QString::number(id));

                enabledGameForm();
                break;
            }

            case comNoPair:
            {
                ui->te_info->append("\nПротивник для игры не найден!");
                break;
            }
            // -------------------------------------------------------






            case comCountPlayer:
            {
                int count{};
                in >> count;
                debugToInfo("Количество клиентов на сервере: " + QString::number(count));

                ui->lb_countPlayer->setText(QString::number(count));
                m_countToPlayerServer = count;

                if (count >= ui->hs_countPlayerKNB->value())
                {
                    ui->lb_infoKNB->setText("Играков достаточно!");
                    ui->pb_gameStartKNB->setEnabled(true);
                }
                else
                {
                    ui->lb_infoKNB->setText("Не хвaтает играков на сервере!");
                    visibleGameKNB(false);
                    ui->pb_gameStartKNB->setEnabled(false);
                }

                break;
            }

            // чат  ---------------------------------------
            // общий чат
            case comMessagePull:
            {
                int id{};
                QString str;
                in >> id >> str;
                chatPullClient(id, str);
                break;
            }

            case comMessagePrivateError:
            {
                systemChatInfo("Неудалось отправить сообщение!"
                               "Выбранный клиент не принимает личные сообщения.");
                break;
            }

            // личный чат
            case comMessagePullPrivate:
            {
                int id{};
                QString str;
                in >> id >> str;

                chatPullPrivate(id, str);

                // если личные сообщения не дублируются в общий чат
                if (!m_messagePrivateChat && !m_privatWinChat)
                {
                    QString strInfo = str;
                    strInfo.truncate(10);
                    ui->gb_infoPullMessage->setVisible(true);
                    ui->pb_infoPrivatPull->setText(QString::number(++m_countMessagePrivateChat));

                    if (m_countMessagePrivateChat == 1)
                    {
                        ui->pb_infoPrivatPull->setToolTip("Вам пришло личное сообщение от: "
                        + QString::number(id) + "\n" + strInfo
                        + QString(str.size() > 10 ? "..." : ""));
                    }
                    else
                    {
                        ui->pb_infoPrivatPull->setToolTip("Вам пришло: "
                        + QString::number(m_countMessagePrivateChat) + " личных сообщений. "
                        "Последнее от: " + QString::number(id) + "\n" + strInfo
                        + QString(str.size() > 10 ? "..." : ""));
                    }
                }
                break;
            }

            // сообщение группы
            case comMessagePullGroup:
            {
                int id;
                QString str;
                in >> id >> str;

                // доделать

                break;
            }

            // ---------------------------------------------------------
            // пришел список клинетов
            case comListClient:
            {
                in >> m_playerList;

                debugToInfo("Статус клиента изменился");

                redrawListClient();
                break;
            }
            // --------------------------------------------------------


            // -------------------------------------------------------------------
            // группа
            // приглашение в группу
            case comOfferGroup:
            {
                int idOpp{};
                QString group;
                in >> idOpp >> group;

                offerToJoin(idOpp, group, true);

                debugToInfo("Пришла команда для приглашения в группу");
                debugToInfo("idOpp = " + QString::number(idOpp));
                debugToInfo("group = " + group);
                break;
            }

            // клиент уже в группе
            case comNotOfferGroup:
            {
                systemChatInfo("Данный клиент уже состоит в этой группе.");
                break;
            }

            // ошибка при приглашении в группу
            case comErrorOfferGroup:
            {
                systemChatInfo("\n Внимание! На сервере призошла ошибка"
                               " при попытки добавить клиента в группу!\n");
                break;
            }

            // клиента исключили (команда от сервера на исключение)
            case comDelClientGroup:
            {
                int idHost{};
                QString group;
                in >> idHost >> group;

                debugToInfo("group - " + group);
                systemChatInfo("Id: " + QString::number(idHost)
                               + " исключил вас из группы: " + group);

                chekDelClientGroup(group);
                break;
            }

            // клиент успешно исключён из группы (ответ сервера о исключении)
            case comDelClientGrouoGood:
            {
                QString strGroup;
                in >> strGroup;

                if (strGroup == "group#1")  // группа 1
                {
                    vecGropu_1.clear();
                    in >> vecGropu_1;
                    textClientLabelGroup(ui->lb_id_groups, vecGropu_1, m_idHost_1, m_hostGroup_1);
                    chekClientGroup();
                }
                else if (strGroup == "group#2")  // группа 2
                {
                    vecGropu_2.clear();
                    in >> vecGropu_2;
                    textClientLabelGroup(ui->lb_id_groups_2, vecGropu_2, m_idHost_2, m_hostGroup_2);
                    chekClientGroup();
                }
                else if (strGroup == "group#3")  // группа 3
                {
                    vecGropu_3.clear();
                    in >> vecGropu_3;
                    textClientLabelGroup(ui->lb_id_groups_3, vecGropu_3, m_idHost_3, m_hostGroup_3);
                    chekClientGroup();
                }
                systemChatInfo("клиент успешно исключён из группы: " + strGroup);
                break;
            }

            // группа расформированна   (или если клиент сам покинул группу)
            case comDeleteGroup:
            {
                QString group;
                in >> group;
                chekDelClientGroup(group);
                systemChatInfo("Группа расформированна");
                break;
            }

            // возникла ошибка при исключении клиента из группы
            case comDelClientGroupError:
            {
                QString strError;
                in >> strError;
                systemChatInfo("При удаление клиента произошла ошибка!");
                debugToInfo(strError, 1);
                break;
            }
            // -------------------------------------------------------------------

            default:
            {
                debugToInfo("ОШИБКА!!! Неизвесная команда "
                             + QString::number(command), 1);
                break;
            }
        }
    }
}

