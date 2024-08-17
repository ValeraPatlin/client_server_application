#include "myclient.h"
#include "ui_myclient.h"



// -----------------------------------------------------------------------
// блокировка чата для оффлайн режима
/*
 * gb_chatRB        - переключение чатов
 * gb_buttonChat    - кнопки управления чатом   (доступны)
 * gb_mesegChat     - отправка сообщения
 *
 * cb_totalChat     - принятие сообщений в личный чат
 * cb_privatChat    - принятие личных сообшений
 *
 * pb_chatPrivatemaesage - кнопка для переключения чата в приватный режим
 */
void MyClient::chatBlok(bool flag)
{
    ui->gb_chatRB->setEnabled(flag);
    ui->gb_mesegChat->setEnabled(flag); // если при старте выбран личный чат
    ui->cb_totalChat->setEnabled(flag);
    ui->cb_privatChat->setEnabled(flag);
    ui->pb_chatPrivatemaesage->setEnabled(flag);
}
// -----------------------------------------------------------------------



// отправка сообщения
void MyClient::on_pb_chatPush_clicked()
{
    QString str = ui->le_message->text();

    if (str.simplified() != "")
    {
        QTextCursor cursor = ui->te_info->textCursor();
        QTextCharFormat format;
        QString textMessage;

        if (!m_privateclick && ui->cb_nameChatGroup->currentIndex() == 0)    // общий чат
        {
            sendToServer(comMessagePush);

            format.setFont(m_chatPushFont);
            format.setForeground(m_colorChatPush);
            cursor.setCharFormat(format);

            textMessage = "Вы:  " + str + "\n";
            cursor.insertText(textMessage);
        }
        else if (m_privateclick && ui->cb_nameChatGroup->currentIndex() == 0)    // личные сообщения
        {
            QString strId = m_listClient.at(0);
            sendToServer(comMessagePushPrivat, strId.toInt());

            textMessage = "Вы пишете "+ strId + ":  " + str + "\n";

            format.setFont(m_chatPrivateFont);
            format.setForeground(m_colorChatPrivate);

            if (m_messagePrivateChat)
            {
                cursor.setCharFormat(format);
                cursor.insertText(textMessage);
            }

            QTextCursor cursor_total = ui->te_privatChat->textCursor();
            cursor_total.setCharFormat(format);
            cursor_total.insertText(textMessage);
        }
        else
        {
            sendToServer(comMessagePushGroup);

            textMessage = "Вы пишете в группу "
                    + ui->cb_nameChatGroup->currentText() + ":  " + str + "\n";

            format.setFont(m_chatGroupFont);
            format.setForeground(m_colorChatGroup);

            if (m_messageGroupChat)
            {
                cursor.setCharFormat(format);
                cursor.insertText(textMessage);
            }

            QTextCursor cursor_total = ui->te_groupChat->textCursor();
            cursor_total.setCharFormat(format);
            cursor_total.insertText(textMessage);
        }

        ui->le_message->clear();
    }
    else
    {
        systemChatInfo("Сообщение не отправленно так как содержит пустую строку.");
    }
}

// oтправить личное сообщение
void MyClient::on_pb_chatPrivatemaesage_clicked()
{
    // если стоит галочка, что принимаются личные сообщения и выбранна общая группа
    if (m_privatChat && ui->cb_nameChatGroup->currentIndex() == 0)
    {
        if (!m_privateclick)    // переключение в личный чат
        {
            m_privateclick = true;
            ui->pb_chatPrivatemaesage->setText("Переключиться на: общий чат");
            ui->pb_chatPrivatemaesage->setToolTip("Ваше сообщение отправиться в личный чат."
                                                  " Переключиться на общий чат.");

            if (m_listClient.size() != 0 && QString::number(idClient) != m_listClient.at(0))
            {
                ui->lb_messageText->setText("Ваше сообщение отправиться в личный чат: id - "
                                            + m_listClient.at(0) + " name - " + m_listClient.at(1));
            }
            else
            {
                ui->lb_messageText->setText("Ваше сообщение отправиться в личный чат: "
                                            "Выберете клиента");
                ui->gb_mesegChat->setEnabled(false);
            }
        }
        else    // общий чат
        {
            switchChatInfo();
        }
    }
    else if (ui->cb_nameChatGroup->currentIndex() == 0)
    {
        systemChatInfo("Вы не можете принимать и отправлять личные сообщения. "
                       "Что бы начать принимать сообщения поставти "
                       "галочку в соответствующее поле");
    }
    else
    {
        systemChatInfo("Вы не можете переключиться на личный чат находясь в групповом чате."
                       "Что бы переключиться на личный чат выберете в меню соответствующий пункт");
    }
}

// сообщения при переключении чата на общий
void MyClient::switchChatInfo()
{
    m_privateclick = false;
    ui->lb_messageText->setText("Ваше сообщение отправиться в общий чат");
    ui->pb_chatPrivatemaesage->setText("Переключиться на: приватный чат");
    ui->pb_chatPrivatemaesage->setToolTip("Ваше сообщение отправиться в общий чат. "
                                          "Переключиться на приватный чат.");
    ui->gb_mesegChat->setEnabled(true);
}

// информация, что пришло личное сообшение, если отключенно дублирование
void MyClient::on_pb_infoPrivatPull_clicked()
{
    on_rb_privatChat_clicked();

    ui->rb_totalChat->setChecked(m_infoWinChat);
    ui->rb_privatChat->setChecked(m_privatWinChat);
    ui->rb_groupChat->setChecked(m_groupWinChat);
}


// принятие сообщение клиентам  ------------------
// общий чат
void MyClient::chatPullClient(int id, QString str)
{
    if (id != idClient)
    {
        QTextCursor cursor = ui->te_info->textCursor();
        QTextCharFormat format;
        format.setFont(m_chatPullFont);
        format.setForeground(m_colorChatPull);
        cursor.setCharFormat(format);

        QString textMessage;
        textMessage = QString::number(id) + ":  " + str + "\n";
        cursor.insertText(textMessage);
    }
}

// личный чат
void MyClient::chatPullPrivate(int id, QString str)
{
    QTextCursor cursor = ui->te_privatChat->textCursor();
    QTextCharFormat format;
    format.setFont(m_chatPrivateFont);
    format.setForeground(m_colorChatPrivate);
    cursor.setCharFormat(format);

    QString textMessage;
    textMessage = "Вам пишет " + QString::number(id) + ":  " + str + "\n";
    cursor.insertText(textMessage);

    if (m_messagePrivateChat)
    {
        QTextCursor cursor_total = ui->te_info->textCursor();
        cursor_total.setCharFormat(format);
        cursor_total.insertText(textMessage);
    }
}
// -------


// переключение на общий чат
void MyClient::on_rb_totalChat_clicked()
{
    ui->te_info->setVisible(true);
    ui->te_privatChat->setVisible(false);
    ui->te_groupChat->setVisible(false);

    m_infoWinChat = true;
    m_privatWinChat = false;
    m_groupWinChat = false;

    if (m_streamChatOnOff && m_privateclick)
    {
        on_pb_chatPrivatemaesage_clicked();
    }

    writeConfig();
}

// переключение на личный чат
void MyClient::on_rb_privatChat_clicked()
{
    ui->te_info->setVisible(false);
    ui->te_privatChat->setVisible(true);
    ui->te_groupChat->setVisible(false);

    m_infoWinChat = false;
    m_privatWinChat = true;
    m_groupWinChat = false;

    // сброс информации о личных сообщениях
    m_countMessagePrivateChat = 0;
    ui->gb_infoPullMessage->setVisible(false);

    if (m_streamChatOnOff && !m_privateclick)
    {
        on_pb_chatPrivatemaesage_clicked();
    }

    writeConfig();
}


// переключение на групповой чат
void MyClient::on_rb_groupChat_clicked()
{
    ui->te_info->setVisible(false);
    ui->te_privatChat->setVisible(false);
    ui->te_groupChat->setVisible(true);

    m_infoWinChat = false;
    m_privatWinChat = false;
    m_groupWinChat = true;
    writeConfig();
}
// ----------------------------------------------------------------------



// --------------------------------------------------------
// переключение статуса
void MyClient::on_cb_statusClient_activated(int index)
{
    debugToInfo("Статус #: " + QString::number(index));

    m_statusClient = index;

    writeConfig();
    sendToServer(comStatusClientChat);
}

// статус можно ли отправлять личные сообщения
void MyClient::statusChatPrivate(bool flag)
{
    if (flag)
    {
        if (m_privatChat)   // если стоит галочка, что принимаются личные сообщения
        {
            ui->lb_statusPrivateChat->setText("Доступно отправление личных сообщений");
            ui->lb_statusPrivateChat->setStyleSheet("QLabel { background: #5fe915; color: black}");
        }
        else
        {
            ui->lb_statusPrivateChat->setText("Невозможно отправить личное сообщение");
            ui->lb_statusPrivateChat->setStyleSheet("QLabel { background: #ff0000; color: black}");
        }
    }
    else
    {
        ui->lb_statusPrivateChat->setText("Клиент отключён от сервера");
        ui->lb_statusPrivateChat->setStyleSheet("QLabel { background: #e7e7e7; color: black}");
    }
}
// --------------------------------------------------------




// ----------------------------------------------------------------------
// Вывод информации на экран клиента
void MyClient::debugToInfo(QString str, int level)
{
    if (m_debugToInfoLevel >= level)
    {
        QString textMessege;
        QTextCursor cursor = ui->te_info->textCursor();
        QTextCharFormat format;
        format.setFont(m_debugFont);
        format.setForeground(m_colordebug);
        cursor.setCharFormat(format);
        str += "\n";

        if (m_dateTimeInfo)
        {
            textMessege = QDateTime::currentDateTime().toString(
                        m_strDateYearInfo
                        + m_strDateMonthInfo
                        + m_strDateDayInfo
                        + m_strDateHourInfo
                        + m_strDateMinuteInfo
                        + m_strDateSecondInfo
                        )
                    + str;
        }
        else
        {
            textMessege = str;
        }

        cursor.insertText(textMessege);

        // для дублирование сообщений в другии чаты
        if (m_copyDebugForChat)
        {
            QTextCursor cursorPriv = ui->te_privatChat->textCursor();
            QTextCharFormat formatPriv;
            formatPriv.setFont(m_debugFont);
            formatPriv.setForeground(m_colordebug);
            cursorPriv.setCharFormat(formatPriv);

            QTextCursor cursorGroup = ui->te_groupChat->textCursor();
            QTextCharFormat formatGroup;
            formatGroup.setFont(m_debugFont);
            formatGroup.setForeground(m_colordebug);
            cursorGroup.setCharFormat(formatGroup);

            cursorPriv.insertText(textMessege);
            cursorGroup.insertText(textMessege);
        }
    }

    // вывод в консоль
    if (m_flagDataToConsole || m_logFileFlag)
    {
        qDebug() << str;
    }
}

// системные сообщения
void MyClient::systemChatInfo(QString str)
{
    if (m_systemChatFlag && str != "")
    {
        QTextCursor cursor = ui->te_info->textCursor();
        QTextCharFormat format;
        format.setFont(m_systemInfoFont);
        format.setForeground(m_colorSystemInfo);
        cursor.setCharFormat(format);
        str += "\n";
        cursor.insertText(str);

        // для дублирование сообщений в другии чаты
        if (m_copySystemForChat)
        {
            QTextCursor cursorPriv = ui->te_privatChat->textCursor();
            QTextCharFormat formatPriv;
            formatPriv.setFont(m_systemInfoFont);
            formatPriv.setForeground(m_colorSystemInfo);
            cursorPriv.setCharFormat(formatPriv);

            QTextCursor cursorGroup = ui->te_groupChat->textCursor();
            QTextCharFormat formatGroup;
            formatGroup.setFont(m_systemInfoFont);
            formatGroup.setForeground(m_colorSystemInfo);
            cursorGroup.setCharFormat(formatGroup);

            cursorPriv.insertText(str);
            cursorGroup.insertText(str);
        }
    }
}
// ------------------------



// ------------------------
// установка вывода даты в дебаг
void MyClient::strDateTimeInfo()
{
    if (m_dateTimeInfo)
    {
        if (m_dateYearInfo)
        {
            m_strDateYearInfo = "yyyy ";
        }
        else
        {
            m_strDateYearInfo = "";
        }

        if (m_dateMonthInfo)
        {
            m_strDateMonthInfo = "MM ";
        }
        else
        {
            m_strDateMonthInfo = "";
        }

        if (m_dateDayInfo)
        {
            m_strDateDayInfo = "dd ";
        }
        else
        {
            m_strDateDayInfo = "";
        }

        if (m_dateHourInfo)
        {
            m_strDateHourInfo = "hh:";
        }
        else
        {
            m_strDateHourInfo = "";
        }

        if (m_dateMinuteInfo)
        {
            m_strDateMinuteInfo = "mm:";
        }
        else
        {
            m_strDateMinuteInfo = "";
        }

        if (m_dateSecondInfo)
        {
            m_strDateSecondInfo = "ss ";
        }
        else
        {
            m_strDateSecondInfo = "";
        }
    }
}

void MyClient::enabledDataTime()
{
    ui->ch_yearInfo->setEnabled(m_dateTimeInfo);
    ui->ch_monthInfo->setEnabled(m_dateTimeInfo);
    ui->ch_dayInfo->setEnabled(m_dateTimeInfo);
    ui->ch_hourInfo->setEnabled(m_dateTimeInfo);
    ui->ch_minuteInfo->setEnabled(m_dateTimeInfo);
    ui->ch_secondInfo->setEnabled(m_dateTimeInfo);
}

void MyClient::on_hs_levelInfo_valueChanged(int value)
{
    ui->lb_numLevel->setNum(value);

    switch (value)
    {
        case 0:
        {
            ui->lb_levelInfoText->setText("Системная информация не выводится");
            break;
        }
        case 1:
        {
            ui->lb_levelInfoText->setText("Выводится только критические ошибки");
            break;
        }
        case 2:
        {
            ui->lb_levelInfoText->setText("Выводится ошибки и важная информация");
            break;
        }
        case 3:
        {
            ui->lb_levelInfoText->setText("Выводится общая информация");
            break;
        }
        case 4:
        {
            ui->lb_levelInfoText->setText("Выводится отладочные данные");
            break;
        }
    }
    m_debugToInfoLevel = value;
    writeConfig();
}


// ---      настройка вывода
void MyClient::on_rb_notDateTime_clicked()
{
    m_dateTimeInfo = false;
    enabledDataTime();

    ui->pb_saveInfoDateDebug->setVisible(true);
    ui->pb_canselDataInfoDebug->setVisible(true);
}

void MyClient::on_rb_dateTimeOut_clicked()
{
    m_dateTimeInfo = true;
    enabledDataTime();

    ui->pb_saveInfoDateDebug->setVisible(true);
    ui->pb_canselDataInfoDebug->setVisible(true);
}


// сохренение
void MyClient::on_pb_saveInfoDateDebug_clicked()
{
    m_temp_dateTimeInfo = m_dateTimeInfo;
    m_temp_dateYearInfo = m_dateYearInfo;
    m_temp_dateMonthInfo = m_dateMonthInfo;
    m_temp_dateDayInfo = m_dateDayInfo;
    m_temp_dateHourInfo = m_dateHourInfo;
    m_temp_dateMinuteInfo = m_dateMinuteInfo;
    m_temp_dateSecondInfo = m_dateSecondInfo;

    enabledDataTime();
    strDateTimeInfo();
    writeConfig();

    ui->pb_saveInfoDateDebug->setVisible(false);
    ui->pb_canselDataInfoDebug->setVisible(false);
}

// отмена
void MyClient::on_pb_canselDataInfoDebug_clicked()
{
    m_dateTimeInfo = m_temp_dateTimeInfo;
    m_dateYearInfo = m_temp_dateYearInfo;
    m_dateMonthInfo = m_temp_dateMonthInfo;
    m_dateDayInfo = m_temp_dateDayInfo;
    m_dateHourInfo = m_temp_dateHourInfo;
    m_dateMinuteInfo = m_temp_dateMinuteInfo;
    m_dateSecondInfo = m_temp_dateSecondInfo;

    if (m_temp_dateTimeInfo)
    {
        ui->rb_dateTimeOut->setChecked(true);
        ui->rb_notDateTime->setChecked(false);
    }
    else
    {
        ui->rb_dateTimeOut->setChecked(false);
        ui->rb_notDateTime->setChecked(true);
    }

    enabledDataTime();

    ui->ch_yearInfo->setChecked(m_temp_dateYearInfo);
    ui->ch_monthInfo->setChecked(m_temp_dateMonthInfo);
    ui->ch_dayInfo->setChecked(m_temp_dateDayInfo);
    ui->ch_hourInfo->setChecked(m_temp_dateHourInfo);
    ui->ch_minuteInfo->setChecked(m_temp_dateMinuteInfo);
    ui->ch_secondInfo->setChecked(m_temp_dateSecondInfo);

    ui->pb_saveInfoDateDebug->setVisible(false);
    ui->pb_canselDataInfoDebug->setVisible(false);

}


// -----  конекты
void MyClient::on_ch_dataTime_clic()
{
    m_dateYearInfo = ui->ch_yearInfo->isChecked();
    m_dateMonthInfo = ui->ch_monthInfo->isChecked();
    m_dateDayInfo = ui->ch_dayInfo->isChecked();
    m_dateHourInfo = ui->ch_hourInfo->isChecked();
    m_dateMinuteInfo = ui->ch_minuteInfo->isChecked();
    m_dateSecondInfo = ui->ch_secondInfo->isChecked();

    ui->pb_saveInfoDateDebug->setVisible(true);
    ui->pb_canselDataInfoDebug->setVisible(true);
}

// ---------------------------------------------------------------------
