#include "myclient.h"
#include "ui_myclient.h"


// ---------------------------------------------------------------
// КНОПКА ЗАПУСК ИГРЫ
void MyClient::on_pb_gameStartKNB_clicked()
{
    debugToInfo("Вызван метод: on_pb_gameStartKNB_clicked");

    if (!offlineMode)   // если флаг оффлайна убран
    {
        if (!m_findStartKNB)
        {
            sendToServer(comSelectionPlayerKNB, 0, "KNB");
            m_findStartKNB= true;
            ui->pb_gameStartKNB->setText("Остановить поиск");
            ui->lb_statusFindGrup->setText("Идет набор играков");
            ui->hs_countPlayerKNB->setEnabled(false);
            ui->sb_countPlayerKNB->setEnabled(false);
            timerFindGroupsAndMotionKNB.start();
        }
        else
        {
            endTimeFindGroupsKNB();
        }
    }
    else
    {
        debugToInfo("Оффлайн режим");
        visibleGameKNB();
    }
}

// кнопка отключение от игры
void MyClient::on_pb_leaveGameKNB_clicked()
{
    if (!offlineMode)   // если флаг оффлайна убран
    {
        visibleGameKNB(false);
        ui->lb_statusFindGrup->setText("");
        delClientGroupKNB();
        sendToServer(comLeaveGroupsKNB);
        ui->hs_countPlayerKNB->setEnabled(true);
        ui->sb_countPlayerKNB->setEnabled(true);

        if (m_timerFormKNBStrart)       // если таймер запущен
        {
            stopTimerFindGroupsAndMotionKNB();
            stopTimerFindGroups();
        }

        blocFormButtonKNB(true);
        infoMotionFormKNB("");
    }
    else
    {
        debugToInfo("Отключение от оффлайн игры");
        visibleGameKNB(false);
        blocFormButtonKNB(true);
        infoMotionFormKNB("");
    }
}


// удаление клиента из группы KNB
void MyClient::delClientGroupKNB()
{
    vecOpponentKNB.clear();
    m_idHost_KNB = 0;
    m_groupKNB = false;
    m_hostGroup_KNB = false;
    ui->lb_myOpponent->setText("Нет данных");
    ui->lb_id_groups_KNB->setText("Нет данных");
    setTextLabelGroup("KNB");
}


// сокрытие игровых кнопок в KNB
void MyClient::visibleGameKNB(bool flag)
{
    ui->pb_stone->setVisible(flag);
    ui->pb_paper->setVisible(flag);
    ui->pb_scissors->setVisible(flag);
    ui->pb_gameStartKNB->setVisible(!flag);
    ui->pb_leaveGameKNB->setVisible(flag);
    ui->pb_gameStartKNB->setEnabled(!flag);
}

// блокировка клавиш при принятия приглашения
void MyClient::enabledGameKNB(bool flag)
{
    debugToInfo("блокировка при принятия приглашения");

    ui->pb_gameStartKNB->setEnabled(flag);
    ui->hs_countPlayerKNB->setEnabled(flag);
    ui->sb_countPlayerKNB->setEnabled(flag);
}
// --------------------------------------------------------------



// --------------------------------------------------------------
// вопрос
void MyClient::questionFormKNB(QString strText,
                               QString strButtonYes,
                               QString strButtomNo)
{
    debugToInfo("Создания вопроса");

    questionFormKNBVisible = true;
    ui->gb_questionKNB->setVisible(true);
    ui->lb_questionFormKNB->setText(strText);
    ui->pb_questionKNBYes->setText(strButtonYes);
    ui->pb_questionKNBNo->setText(strButtomNo);
    ui->pb_gameStartKNB->setEnabled(false);
}

void MyClient::questionFormKNBDefolt()
{
    questionFormKNBVisible = false;
    m_statusFormKNB = statusDefolt;
    ui->gb_questionKNB->setVisible(false);
}

// ответ ДА
void MyClient::on_pb_questionKNBYes_clicked()
{
    switch (m_statusFormKNB)
    {
        case statusDefolt:
        {
            debugToInfo("Ошибка! Нулевой статус! Кнопка Да", 2);
            break;
        }

        case stopFindGroupsKNB:
        {
            sendToServer(comGameStartKNB);
            break;
        }

        case clientLeaveKNB:    // остаться в игре
        {
            visibleGameKNB(true);
            // если сделан ход то запушен таймер - запускаю обратно его
            if (m_motionKNB || m_motionOppjnentKNB)
            {
                sendToServer(comTimerMotionStartStopKNB);
            }
            break;
        }

        default:
        {
            debugToInfo("ОШИБКА!!! Неизвесный статус! Кнопка Да", 1);
            break;
        }
    }
    questionFormKNBDefolt();
}

// ответ нет
void MyClient::on_pb_questionKNBNo_clicked()
{
    switch (m_statusFormKNB)
    {
        case statusDefolt:
        {
            debugToInfo("Ошибка! Нулевой статус!  Кнопка Нет", 2);
            break;
        }

        case stopFindGroupsKNB:
        case clientLeaveKNB:
        {
            sendToServer(comLeaveGroupsKNB);
            visibleGameKNB(false);
            ui->lb_statusFindGrup->setText("");
            delClientGroupKNB();
            ui->hs_countPlayerKNB->setEnabled(true);
            ui->sb_countPlayerKNB->setEnabled(true);
            break;
        }

        default:
        {
            debugToInfo("ОШИБКА!!! Неизвесный статус!  Кнопка Нет", 1);
            break;
        }
    }
    questionFormKNBDefolt();
}
// --------------------------------------------------------------








// --------------------------------------------------------------
// кнопки для игры
void MyClient::on_pb_stone_clicked()
{
    if (!offlineMode)   // если флаг оффлайна убран
    {
        sendToServer(comMotionKNB, -1, "Камень");
        blocFormButtonKNB(false);
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Камень");
        timerFindGroupsAndMotionKNB.start();
        m_motionKNB = true;
    }
    else
    {
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Камень");
        m_codeAnswerPlayerKNB = 1;
        blocFormButtonKNB(false);
        m_offlineGameKNBTimer.start();
        ui->lb_statusInfoStepKNB->setText("Подождите, компьютер думает.");
    }
}

void MyClient::on_pb_scissors_clicked()
{
    if (!offlineMode)   // если флаг оффлайна убран
    {
        sendToServer(comMotionKNB, -1, "Ножницы");
        blocFormButtonKNB(false);
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Ножницы");
        timerFindGroupsAndMotionKNB.start();
        m_motionKNB = true;
    }
    else
    {
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Ножницы");
        m_codeAnswerPlayerKNB = 2;
        blocFormButtonKNB(false);
        m_offlineGameKNBTimer.start();
        ui->lb_statusInfoStepKNB->setText("Подождите, компьютер думает.");
    }
}

void MyClient::on_pb_paper_clicked()
{
    if (!offlineMode)   // если флаг оффлайна убран
    {
        sendToServer(comMotionKNB, -1, "Бумага");
        blocFormButtonKNB(false);
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Бумагу");
        timerFindGroupsAndMotionKNB.start();
        m_motionKNB = true;
    }
    else
    {
        infoMotionFormKNB("Вы сделали ход! \n Вы выбрали: Бумагу");
        m_codeAnswerPlayerKNB = 3;
        blocFormButtonKNB(false);
        m_offlineGameKNBTimer.start();
        ui->lb_statusInfoStepKNB->setText("Подождите, компьютер думает.");
    }
}

void MyClient::blocFormButtonKNB(bool flag)
{
    ui->pb_stone->setEnabled(flag);
    ui->pb_scissors->setEnabled(flag);
    ui->pb_paper->setEnabled(flag);
}

// -------------------------------------------
// вывод информации об игре
void MyClient::infoMotionFormKNB(QString str)
{
    ui->lb_infoGameKNB->setText(str);
    systemChatInfo(str);
}

void MyClient::infoMotionFormKNB(QString str, QVector<int> vec)
{
    for (auto elem : vec)
    {
        if (elem != idClient)
        {
            str += QString::number(elem) + ", ";
        }
    }
    ui->lb_infoGameKNB->setText(str);
    systemChatInfo(str);
}
// ----------------------------------------------------------------------


// ----------------------------------------------------------------------
// настройки игры
void MyClient::on_hs_countPlayerKNB_valueChanged(int value)
{
    ui->sb_countPlayerKNB->setValue(value);

    if (value > m_countToPlayerServer)
    {
        ui->lb_infoKNB->setText("Не хвaтает играков на сервере!");
        ui->pb_gameStartKNB->setEnabled(false);
    }
    else
    {
        ui->lb_infoKNB->setText("Играков достаточно!");
        ui->pb_gameStartKNB->setEnabled(true);
    }
}

void MyClient::on_sb_countPlayerKNB_valueChanged(int arg1)
{
    if (arg1 < 2)
    {
        ui->hs_countPlayerKNB->setValue(2);
        ui->sb_countPlayerKNB->setValue(2);
    }
    else
    {
        ui->hs_countPlayerKNB->setValue(arg1);
    }
}
// ----------------------------------------------------------------------



// ----------------------------------------------------------------------
// таймеры поиска группы и хода игрока (слот)
void MyClient::slotProgressFindGroups()
{
    ui->pb_timerFindGroups->setValue(++m_step);
    ui->lb_stepOpp->setNum(--m_stepOpposite);

    m_timerFormKNBStrart = true;

    if (ui->pb_timerFindGroups->value() == 60)
    {
        if (m_motionOppjnentKNB && !m_motionKNB)    // противник не сделал ход
        {
            stopTimerFindGroupsAndMotionKNB();
            blocFormButtonKNB();
            m_motionOppjnentKNB = false;
            sendToServer(comNotDoneMotionKNB);
            infoMotionFormKNB("Вы проиграли :(");
        }
        else if (m_motionKNB)   // я сделал ход
        {
            stopTimerFindGroupsAndMotionKNB();
            blocFormButtonKNB();
            m_motionKNB = false;
            m_motionOppjnentKNB = false;
        }

        // eсли клиент хост если клиент начал сбор группы
        // во время сбора группы для вывода полоски таймера
        else if (m_clientRoot && !m_motionOppjnentKNB && !m_motionKNB)
        {
            endTimeFindGroupsKNB();
        }
        else
        {
            stopTimerFindGroupsAndMotionKNB();
            m_clientRoot = true;
        }
    }
}



// -----------------------------
void MyClient::stopTimerFindGroupsAndMotionKNB()
{
    debugToInfo("Вызван метод stopTimerFindGroupsAndMotionKNB");
    timerFindGroupsAndMotionKNB.stop();
    m_timerFormKNBStrart = false;
    m_step = 0;
    m_stepOpposite = 60;
    ui->lb_stepOpp->setNum(m_stepOpposite);
    ui->pb_timerFindGroups->setValue(0);
}

void MyClient::stopTimerFindGroups(QString str)
{
    debugToInfo("Таймер поиска остановлен");
    stopTimerFindGroupsAndMotionKNB();

    if (vecOpponentKNB.size() == 0)
    {
        ui->hs_countPlayerKNB->setEnabled(true);
        ui->sb_countPlayerKNB->setEnabled(true);
    }

    ui->lb_statusFindGrup->setText(str);
    m_findStartKNB= false;
    ui->pb_gameStartKNB->setText("ИГРАТЬ");
}

void MyClient::stopTimerFindGroups()
{
    m_motionKNB = false;
    m_motionOppjnentKNB = false;
}
// ---------------------------


// если таймер дошёл до кoнца или нажата кнопка "остановить поиск"
void MyClient::endTimeFindGroupsKNB()
{
    stopTimerFindGroups("Поиск остановлен");
    sendToServer(comStopFindGroupsKNB);

    m_statusFormKNB = stopFindGroupsKNB;

    if (vecOpponentKNB.size() != 0)
    {
        questionFormKNB("Группа для игры неполностью собрана. Играть так?",
                        "Играть так", "Покинуть группу");
    }
}
// ----------------------------------------------------------------------





// ----------------------------------------------------------------------
// игра в оффлайн режиме
void MyClient::slotofflineGameKNB()
{
    ui->pb_timerStatuStepKNB->setValue(++m_stepOffline);

    if (ui->pb_timerStatuStepKNB->value() == m_maxStepPointOffline)
    {
        m_offlineGameKNBTimer.stop();
        blocFormButtonKNB(true);
        m_stepOffline = 0;
        resultOfflineAnswerKNB();
        ui->lb_statusInfoStepKNB->setText("Компьютер сделал ход!");
        ui->pb_timerStatuStepKNB->setValue(m_stepOffline);
    }
    else
    {
        m_answerRandomKNB = QRandomGenerator::global()->bounded(1, 4);
    }
}

/*
 * 1 - камень
 * 2 - ножницы
 * 3 - бумага
 */
void MyClient::resultOfflineAnswerKNB()
{
    if (m_codeAnswerPlayerKNB != 0 && m_answerRandomKNB != 0)
    {
        if (m_codeAnswerPlayerKNB == m_answerRandomKNB)
        {
            infoMotionFormKNB("Ничья  :(");
        }
        else if (m_codeAnswerPlayerKNB == 1 && m_answerRandomKNB == 2)
        {
            infoMotionFormKNB("Вы победили!!!! \n Компьютер выбрал: Ножницы");
        }
        else if (m_codeAnswerPlayerKNB == 1 && m_answerRandomKNB == 3)
        {
            infoMotionFormKNB("Вы проиграли  :( \n Компьютер выбрал: Бумагу");
        }
        else if (m_codeAnswerPlayerKNB == 2 && m_answerRandomKNB == 1)
        {
            infoMotionFormKNB("Вы проиграли  :( \n Компьютер выбрал: Камень");
        }
        else if (m_codeAnswerPlayerKNB == 2 && m_answerRandomKNB == 3)
        {
            infoMotionFormKNB("Вы победили!!!! \n Компьютер выбрал: Бумагу");
        }
        else if (m_codeAnswerPlayerKNB == 3 && m_answerRandomKNB == 1)
        {
            infoMotionFormKNB("Вы победили!!!! \n Компьютер выбрал: Камень");
        }
        else if (m_codeAnswerPlayerKNB == 3 && m_answerRandomKNB == 2)
        {
            infoMotionFormKNB("Вы проиграли  :( \n Компьютер выбрал: Ножницы");
        }

        m_codeAnswerPlayerKNB = 0;
    }
    else
    {
        debugToInfo("В метод resultOfflineAnswerKNB пришел 0 \n "
                    "m_codeAnswerPlayerKNB = " + QString::number(m_codeAnswerPlayerKNB)
                    + "\n m_answerRandomKNB = " + QString::number(m_answerRandomKNB), 1);
    }
}
// ----------------------------------------------------------------------




// -----------------------------------------------------------------------
// добавление бота
void MyClient::on_cb_botPushKNB_clicked()
{
    //
}
// -----------------------------------------------------------------------
