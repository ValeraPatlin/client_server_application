#include "myclient.h"
#include "ui_myclient.h"


// ---------------------------------------------------------------------
// доп настройки показать
void MyClient::on_cb_isSettings_clicked()
{
    if (ui->cb_isSettings->isChecked())
    {
        ui->gb_settingsAd->setVisible(true);
        ui->gb_settings->setMaximumHeight(350);
    }
    else
    {
        ui->gb_settingsAd->setVisible(false);
        ui->gb_settings->setMaximumHeight(180);
    }
}
// очистка конфига
// выбор чата

// ------------------------------
// вывод пароля
void MyClient::on_rb_pass_clicked()     // показывать ***
{
    ui->le_passwd->setEchoMode(QLineEdit::Password);
    passwdEchoMode = 1;
    writeConfig();
}

void MyClient::on_rb_passEcho_clicked()     // при потере фокуса
{
    ui->le_passwd->setEchoMode(QLineEdit::PasswordEchoOnEdit);
    passwdEchoMode = 2;
    writeConfig();
}

void MyClient::on_rb_passNormal_clicked()    // показывать полностью
{
    ui->le_passwd->setEchoMode(QLineEdit::Normal);
    passwdEchoMode = 3;
    writeConfig();
}

void MyClient::on_rb_passNoEcho_clicked()   // скрыть полностью
{
    ui->le_passwd->setEchoMode(QLineEdit::NoEcho);
    passwdEchoMode = 4;
    writeConfig();
}
// -------------------------
// ----------------------------------------------------------------------


// ---------------------------------------------------------------------
// флажки на форме авторизации
void MyClient::on_cb_loginSave_clicked()    // запомнить логин
{
    if (ui->cb_loginSave->isChecked())
    {
        systemChatInfo("Внимание. Теперь ваш логин будет сохранён в файле client.conf");
    }
    else
    {
        ui->cb_passwdSave->setChecked(false);
    }
}

void MyClient::on_cb_passwdSave_clicked()   // запомнить пароль
{
    if (ui->cb_passwdSave->isChecked())
    {
        systemChatInfo("Внимание. Теперь ваш логин и пароль будет сохранёны в файле client.conf");
        ui->cb_loginSave->setChecked(true);
    }
}
// ---------------------------------------------------------------------


// ---------------------------------------------------------------------
// изменение растояния между элементами в списке клиентов
void MyClient::on_sb_sizeItemListGame_valueChanged(int arg1)
{
    debugToInfo("Pастояния между элементами: " + QString::number(arg1));
    ui->gameListWidget->setSpacing(arg1);
    m_numSpacingListGame = arg1;
    writeConfig();
}

// ----     выбор цвета
void MyClient::on_pb_colorOnline_clicked()
{
    m_colorGoPlaye = QColorDialog::getColor(m_colorGoPlaye, this,
                                            "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorGoPlaye.isValid())
    {
        debugToInfo("Цвет: " + m_colorGoPlaye.name());

        m_temp_colorGoPlaye = m_colorGoPlaye;
        m_paletteGoPlaye.setColor(QPalette::Button, m_colorGoPlaye);
        ui->pb_colorOnline->setAutoFillBackground(true);
        ui->pb_colorOnline->setPalette(m_paletteGoPlaye);
        ui->pb_colorOnline->update();
        redrawListClient();
        writeConfig();
    }
    else
    {
        m_colorGoPlaye = m_temp_colorGoPlaye;
    }
}


void MyClient::on_pb_colorChat_clicked()
{
    m_colorChat = QColorDialog::getColor(m_colorChat, this,
                                            "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorChat.isValid())
    {
        debugToInfo("Цвет: " + m_colorChat.name());

        m_temp_colorChat = m_colorChat;
        m_paletteChat.setColor(QPalette::Button, m_colorChat);
        ui->pb_colorChat->setAutoFillBackground(true);
        ui->pb_colorChat->setPalette(m_paletteChat);
        ui->pb_colorChat->update();
        redrawListClient();
        writeConfig();
    }
    else
    {
        m_colorChat = m_temp_colorChat;
    }
}

void MyClient::on_pb_colorBusy_clicked()
{
    m_colorBusy = QColorDialog::getColor(m_colorBusy, this,
                                            "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorBusy.isValid())
    {
        debugToInfo("Цвет: " + m_colorBusy.name());

        m_temp_colorBusy = m_colorBusy;
        m_paletteBusy.setColor(QPalette::Button, m_colorBusy);
        ui->pb_colorBusy->setAutoFillBackground(true);
        ui->pb_colorBusy->setPalette(m_paletteBusy);
        ui->pb_colorBusy->update();
        redrawListClient();
        writeConfig();
    }
    else
    {
        m_colorBusy = m_temp_colorBusy;
    }
}
// ----------



// перерисовать список клиентов
void MyClient::redrawListClient()
{
    ui->gameListWidget->clear();
    QListWidgetItem* elem = nullptr;

    int i = 0;

    for (auto str : m_playerList)
    {
        elem = new QListWidgetItem(str, ui->gameListWidget);
        QStringList list = elem->text().split(" ");

        if (list.at(4) == "занят")
        {
            ui->gameListWidget->item(i++)->setBackgroundColor(m_colorBusy);
        }
        else if (list.at(4) == "только")
        {
            ui->gameListWidget->item(i++)->setBackgroundColor(m_colorChat);
        }
        else
        {
            ui->gameListWidget->item(i++)->setBackgroundColor(m_colorGoPlaye);
        }
    }
}
// ---------------------------------------------------------------------


// --------------------------------------------------------------------
// настройки подключения

// пинг сервера
void MyClient::on_pb_ping_clicked()
{
    QHostInfo hostInfo = QHostInfo::fromName(ui->le_ipAddress->text());
    debugToInfo("ip адрес: " + ui->le_ipAddress->text(), 3);

    if (hostInfo.error() != QHostInfo::NoError)
    {
        debugToInfo("Ошибка при пинге:  " + hostInfo.errorString(), 2);
        systemChatInfo("Не удалось выполнить команду пинг");
        ui->lb_infoPing->setStyleSheet("QLabel { background: #FF0000; color: black}");
        ui->lb_infoPing->setText("  Ping ERROR");
    }
    else
    {
        systemChatInfo("Команда пинг выполнена удачно");
        ui->lb_infoPing->setStyleSheet("QLabel { background: #00FF00; color: black}");
        ui->lb_infoPing->setText("  Ping OK");
    }
}

// установить локальный ip адрес
void MyClient::on_pb_localhost_clicked()
{
    ui->le_ipAddress->setText("127.0.0.1");
    strIPAddres = "127.0.0.1";
    ui->pb_saveIpAdressAndPort->setVisible(false);
    ui->pb_cancel_ipAdressAndPort->setVisible(false);
    writeConfig();
}

// изменение строки ip адрес
void MyClient::on_le_ipAddress_textEdited(const QString &arg1)
{
    debugToInfo(arg1);
    strIPAddres = arg1;
    ui->pb_saveIpAdressAndPort->setVisible(true);
    ui->pb_cancel_ipAdressAndPort->setVisible(true);
}

// изменение строки порта
void MyClient::on_le_port_textEdited(const QString &arg1)
{
    debugToInfo(arg1);
    m_port = arg1.toInt();
    ui->pb_saveIpAdressAndPort->setVisible(true);
    ui->pb_cancel_ipAdressAndPort->setVisible(true);
}

// сохранить
void MyClient::on_pb_saveIpAdressAndPort_clicked()
{
    m_tempStrIPAddress = strIPAddres;
    m_tempPort = m_port;
    writeConfig();
    ui->pb_saveIpAdressAndPort->setVisible(false);
    ui->pb_cancel_ipAdressAndPort->setVisible(false);
}

// отмена
void MyClient::on_pb_cancel_ipAdressAndPort_clicked()
{
    strIPAddres = m_tempStrIPAddress;
    m_port = m_tempPort;
    ui->le_ipAddress->setText(m_tempStrIPAddress);
    ui->le_port->setText(QString::number(m_tempPort));
    ui->pb_saveIpAdressAndPort->setVisible(false);
    ui->pb_cancel_ipAdressAndPort->setVisible(false);
}
// ----------------------------------------------------------------------



// ---------------------------------------------------------------------
// логер

// создание логера
void MyClient::on_cb_logFile_clicked()
{
    if (ui->cb_logFile->isChecked())
    {
        ui->le_pathLogFile->setEnabled(true);
        ui->pb_dirPathLog->setEnabled(true);
        ui->le_pathLogFile->setText("./game_client.log");
        ui->lb_logInfo->setText("Данные в логер будут записаны при следующем старте программы");
    }
    else
    {
        ui->le_pathLogFile->setEnabled(false);
        ui->pb_dirPathLog->setEnabled(false);
        ui->le_pathLogFile->setText("");
    }

    ui->rb_logConsol->setChecked(ui->cb_logFile->isChecked());
    m_logFileFlag = ui->cb_logFile->isChecked();
    m_tempLogFileFlag = (!ui->cb_logFile->isChecked());
    m_pathAndNameLogFile = ui->le_pathLogFile->text();
    ui->pb_saveLogFile->setVisible(ui->cb_logFile->isChecked());
    ui->pb_cancelLogFile->setVisible(ui->cb_logFile->isChecked());
    ui->lb_logInfo->setVisible(ui->cb_logFile->isChecked());
    writeConfig();
}

// не выводить данные в консоль
void MyClient::on_rb_logConsol_clicked()
{
    debugToInfo("Данные в консоль не выводяться");
    m_flagDataToConsole = false;
    ui->lb_logInfo->setVisible(false);
    ui->pb_saveLogFile->setVisible(true);
    ui->pb_cancelLogFile->setVisible(true);
}

// отправка данных в консоль
void MyClient::on_rb_textLogFile_clicked()
{
    systemChatInfo("Данные будут отправленны в консоль!");

    if (ui->cb_logFile->isChecked() && ui->rb_textLogFile->isChecked())
    {
        systemChatInfo("Внимание! Данные будут записаны в log файл и отправленны в консоль!");
        ui->lb_logInfo->setVisible(true);
        ui->lb_logInfo->setText("Внимание! Данные будут записаны в "
                                "log файл и отправленны в консоль!");
    }
    m_flagDataToConsole = true;
    ui->pb_saveLogFile->setVisible(true);
    ui->pb_cancelLogFile->setVisible(true);
}

// сохранить
void MyClient::on_pb_saveLogFile_clicked()
{
    debugToInfo("флаг логера: " + QString::number(m_logFileFlag));
    debugToInfo("Путь log файла: " + m_pathAndNameLogFile, 3);

    m_logFileFlag = ui->cb_logFile->isChecked();
    m_tempLogFileFlag = ui->cb_logFile->isChecked();
    m_tempPathAndNameLogFile = ui->le_pathLogFile->text();
    m_tempGlagDataToConsol = m_flagDataToConsole;
    ui->pb_saveLogFile->setVisible(false);
    ui->pb_cancelLogFile->setVisible(false);
    writeConfig();
}

// отмена
void MyClient::on_pb_cancelLogFile_clicked()
{
    ui->cb_logFile->setChecked(m_tempLogFileFlag);
    ui->le_pathLogFile->setText(m_tempPathAndNameLogFile);
    m_pathAndNameLogFile = m_tempPathAndNameLogFile;
    m_flagDataToConsole = m_tempGlagDataToConsol;

    ui->pb_saveLogFile->setVisible(false);
    ui->pb_cancelLogFile->setVisible(false);
}

// путь до логера
void MyClient::on_le_pathLogFile_textEdited(const QString &arg1)
{
    debugToInfo(arg1);
    m_pathAndNameLogFile += arg1;
    ui->pb_saveLogFile->setVisible(true);
    ui->pb_cancelLogFile->setVisible(true);
}

// кнопка выбора пути
void MyClient::on_pb_dirPathLog_clicked()
{
    QString strDir = QFileDialog::getExistingDirectory(this,
                                                       "Выберете катадог для сохранения логера",
                                                       QDir::currentPath());

    ui->le_pathLogFile->setText(strDir + "/game_client.log");
    m_pathAndNameLogFile = ui->le_pathLogFile->text();
    ui->pb_saveLogFile->setVisible(true);
    ui->pb_cancelLogFile->setVisible(true);
}

// -------
// получение данных
bool MyClient::getFlagLogFile()
{
    return m_logFileFlag;
}

QString MyClient::getPathLogFile()
{
    return m_pathAndNameLogFile;
}
// ---------------------------------------------------------------------




// -----------------------------------------------------------------------
// чат
void MyClient::on_cb_systemChat_clicked()
{
    m_systemChatFlag = ui->cb_systemChat->isChecked();
    writeConfig();
}

void MyClient::on_cb_totalChat_clicked()
{
    m_totalChat = ui->cb_totalChat->isChecked();
    writeConfig();
    sendToServer(comGeneralChatOnOff);
}

// чат с личными сообщениями (принимает ли клиент личные сообщения)
void MyClient::on_cb_privatChat_clicked()
{
    m_privatChat = ui->cb_privatChat->isChecked();
    statusChatPrivate();
    sendToServer(comPrivateChatOnOff);
    writeConfig();

    if (m_privatWinChat)
    {
        on_rb_totalChat_clicked();
        ui->rb_privatChat->setChecked(false);
        ui->rb_totalChat->setChecked(true);

        if (m_privateclick)
        {
            switchChatInfo();
        }
    }
}

// Показывать личные сообщения в общем чате
void MyClient::on_cb_messagePrivatTotalChat_clicked()
{
    m_messagePrivateChat = ui->cb_messagePrivatTotalChat->isChecked();
    writeConfig();
}

// Показывать сообщения группы в общем чате
void MyClient::on_cb_messageGroupTotalChat_clicked()
{
    m_messageGroupChat = ui->cb_messageGroupTotalChat->isChecked();
    writeConfig();
}

// При перекдючении чатов - переключать канал сообщений
void MyClient::on_ch_streamChat_clicked()
{
    m_streamChatOnOff = ui->ch_streamChat->isChecked();
    writeConfig();
}
// ------------------------------------------------------------------------



// ---------------------------------------------------------------------
// изменения чата

// системные сообщения -------
void MyClient::on_pb_colorSystemInfo_clicked()
{
    m_colorSystemInfo = QColorDialog::getColor(m_colorSystemInfo, this,
                                              "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorSystemInfo.isValid())
    {
        debugToInfo("(системные сообщения) Цвет: " + m_colorSystemInfo.name());

        m_temp_colorSystemInfo = m_colorSystemInfo;
        m_paletteSystemInfo.setColor(QPalette::Button, m_colorSystemInfo);
        ui->pb_colorSystemInfo->setAutoFillBackground(true);
        ui->pb_colorSystemInfo->setPalette(m_paletteSystemInfo);
        ui->pb_colorSystemInfo->update();
        writeConfig();
    }
    else
    {
        m_colorSystemInfo = m_temp_colorSystemInfo;
    }
}

void MyClient::on_cb_systemInfoFondSize_activated(int index)
{
    m_systemInfoIndexFond = index;
    m_systemInfoFont.setPointSize(ui->cb_systemInfoFondSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_systemInfoFond_clicked()
{
    m_systemInfoFont = QFontDialog::getFont(&m_systemInfoFontSelected, m_systemInfoFont,
                                                 this, "Выберети шрифт");

    if (m_systemInfoFontSelected)
    {
        if (m_systemInfoFont.pointSize() < 10)
        {
            ui->cb_systemInfoFondSize->setCurrentIndex(0);
            m_systemInfoFont.setPointSize(10);
        }
        else if (m_systemInfoFont.pointSize() > 18)
        {
            ui->cb_systemInfoFondSize->setCurrentIndex(8);
            m_systemInfoFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_systemInfoFondSize->findText(QString::number(
                                                            m_systemInfoFont.pointSize()),
                                                        Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_systemInfoFondSize->setCurrentIndex(0);
                m_systemInfoFont.setPointSize(10);
            }
            else
            {
                ui->cb_systemInfoFondSize->setCurrentIndex(value);
                m_systemInfoIndexFond = value;
                ui->pb_systemInfoFond->setText(m_systemInfoFont.family());
            }
        }

        if (m_systemInfoFont.weight() > 50)
        {
            m_weightFontSystemInfo = true;
        }
        else
        {
            m_weightFontSystemInfo = false;
        }

        if (m_systemInfoFont.italic())
        {
            m_italicFontSystemInfo = true;
        }
        else
        {
            m_italicFontSystemInfo = false;
        }

        if (m_systemInfoFont.underline())
        {
            m_underlineFontSystemInfo = true;
        }
        else
        {
            m_underlineFontSystemInfo = false;
        }

        ui->cb_weightFontSystemInfo->setChecked(m_weightFontDebug);
        ui->cb_italicFontSystemInfo->setChecked(m_italicFontDebug);
        ui->cb_underlineFontSystemInfo->setChecked(m_underlineFontDebug);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontSystemInfo_clicked()
{
    if (ui->cb_weightFontSystemInfo->isChecked())
    {
        m_weightFontSystemInfo = true;
    }
    else
    {
        m_weightFontSystemInfo = false;
    }
    m_systemInfoFont.setBold(m_weightFontSystemInfo);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontSystemInfo_clicked()
{
    if (ui->cb_italicFontSystemInfo->isChecked())
    {
        m_italicFontSystemInfo = true;
    }
    else
    {
        m_italicFontSystemInfo = false;
    }
    m_systemInfoFont.setItalic(m_italicFontSystemInfo);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontSystemInfo_clicked()
{
    if (ui->cb_underlineFontSystemInfo->isChecked())
    {
        m_underlineFontSystemInfo = true;
    }
    else
    {
        m_underlineFontSystemInfo = false;
    }
    m_systemInfoFont.setUnderline(m_underlineFontSystemInfo);
    writeConfig();
}


// debug ------
void MyClient::on_pb_colorDebug_clicked()
{
    m_colordebug = QColorDialog::getColor(m_colordebug, this,
                                          "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colordebug.isValid())
    {
        debugToInfo("(debug) Цвет: " + m_colordebug.name());

        m_temp_colordebug = m_colordebug;
        m_palettedebug.setColor(QPalette::Button, m_colordebug);
        ui->pb_colorDebug->setAutoFillBackground(true);
        ui->pb_colorDebug->setPalette(m_palettedebug);
        ui->pb_colorDebug->update();
        writeConfig();
    }
    else
    {
        m_colordebug = m_temp_colordebug;
    }
}

void MyClient::on_cb_debugFondSize_activated(int index)
{
    m_debugIndexFond = index;
    m_debugFont.setPointSize(ui->cb_debugFondSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_debugFond_clicked()
{
    m_debugFont = QFontDialog::getFont(&m_debugFontSelected, m_debugFont,
                                       this, "Выберети шрифт");

    if (m_debugFontSelected)
    {
        if (m_debugFont.pointSize() < 10)
        {
            ui->cb_debugFondSize->setCurrentIndex(0);
            m_debugFont.setPointSize(10);
        }
        else if (m_debugFont.pointSize() > 18)
        {
            ui->cb_debugFondSize->setCurrentIndex(8);
            m_debugFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_debugFondSize->findText(QString::number(m_debugFont.pointSize()),
                                                   Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_debugFondSize->setCurrentIndex(0);
                m_debugFont.setPointSize(10);
            }
            else
            {
                ui->cb_debugFondSize->setCurrentIndex(value);
                m_debugIndexFond = value;
                ui->pb_debugFond->setText(m_debugFont.family());
            }
        }

        if (m_debugFont.weight() > 50)
        {
            m_weightFontDebug = true;
        }
        else
        {
            m_weightFontDebug = false;
        }

        if (m_debugFont.italic())
        {
            m_italicFontDebug = true;
        }
        else
        {
            m_italicFontDebug = false;
        }

        if (m_debugFont.underline())
        {
            m_underlineFontDebug = true;
        }
        else
        {
            m_underlineFontDebug = false;
        }

        ui->cb_weightFontDebug->setChecked(m_weightFontDebug);
        ui->cb_italicFontDebug->setChecked(m_italicFontDebug);
        ui->cb_underlineFontDebug->setChecked(m_underlineFontDebug);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontDebug_clicked()
{
    if (ui->cb_weightFontDebug->isChecked())
    {
        m_weightFontDebug = true;
    }
    else
    {
        m_weightFontDebug = false;
    }
    m_debugFont.setBold(m_weightFontDebug);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontDebug_clicked()
{
    if (ui->cb_italicFontDebug->isChecked())
    {
        m_italicFontDebug = true;
    }
    else
    {
        m_italicFontDebug = false;
    }
    m_debugFont.setItalic(m_italicFontDebug);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontDebug_clicked()
{
    if (ui->cb_underlineFontDebug->isChecked())
    {
        m_underlineFontDebug = true;
    }
    else
    {
        m_underlineFontDebug = false;
    }
    m_debugFont.setUnderline(m_underlineFontDebug);
    writeConfig();
}



// отправленные сообщения   -----------------
void MyClient::on_pb_colorChatPush_clicked()
{
    m_colorChatPush = QColorDialog::getColor(m_colorChatPush, this,
                                             "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorChatPush.isValid())
    {
        debugToInfo("(ChatPush) Цвет: " + m_colorChatPush.name());

        m_temp_colorChatPush = m_colorChatPush;
        m_paletteChatPush.setColor(QPalette::Button, m_colorChatPush);
        ui->pb_colorChatPush->setAutoFillBackground(true);
        ui->pb_colorChatPush->setPalette(m_paletteChatPush);
        ui->pb_colorChatPush->update();
        writeConfig();
    }
    else
    {
        m_colorChatPush = m_temp_colorChatPush;
    }
}

void MyClient::on_cb_chatPushFontSize_activated(int index)
{
    m_chatPushIndexFond = index;
    m_chatPushFont.setPointSize(ui->cb_chatPushFontSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_chatPushFont_clicked()
{
    m_chatPushFont = QFontDialog::getFont(&m_chatPushFontSelected, m_chatPushFont,
                                          this, "Выберети шрифт");

    if (m_chatPushFontSelected)
    {
        if (m_chatPushFont.pointSize() < 10)
        {
            ui->cb_chatPushFontSize->setCurrentIndex(0);
            m_chatPushFont.setPointSize(10);
        }
        else if (m_chatPushFont.pointSize() > 18)
        {
            ui->cb_chatPushFontSize->setCurrentIndex(8);
            m_chatPushFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_chatPushFontSize->findText(QString::number(m_chatPushFont.pointSize()),
                                                   Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_chatPushFontSize->setCurrentIndex(0);
                m_chatPushFont.setPointSize(10);
            }
            else
            {
                ui->cb_chatPushFontSize->setCurrentIndex(value);
                m_chatPushIndexFond = value;
                ui->pb_chatPushFont->setText(m_chatPushFont.family());
            }
        }

        if (m_chatPushFont.weight() > 50)
        {
            m_weightFontchatPush = true;
        }
        else
        {
            m_weightFontchatPush = false;
        }

        if (m_chatPushFont.italic())
        {
            m_italicFontchatPush = true;
        }
        else
        {
            m_italicFontchatPush = false;
        }

        if (m_chatPushFont.underline())
        {
            m_underlineFontchatPush = true;
        }
        else
        {
            m_underlineFontchatPush = false;
        }

        ui->cb_weightFontChatPush->setChecked(m_weightFontchatPush);
        ui->cb_italicFontChatPush->setChecked(m_italicFontchatPush);
        ui->cb_underlineFontChatPush->setChecked(m_underlineFontchatPush);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontChatPush_clicked()
{
    if (ui->cb_weightFontChatPush->isChecked())
    {
        m_weightFontchatPush = true;
    }
    else
    {
        m_weightFontchatPush = false;
    }
    m_chatPushFont.setBold(m_weightFontchatPush);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontChatPush_clicked()
{
    if (ui->cb_italicFontChatPush->isChecked())
    {
        m_italicFontchatPush = true;
    }
    else
    {
        m_italicFontchatPush = false;
    }
    m_chatPushFont.setItalic(m_italicFontchatPush);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontChatPush_clicked()
{
    if (ui->cb_underlineFontChatPush->isChecked())
    {
        m_underlineFontchatPush = true;
    }
    else
    {
        m_underlineFontchatPush = false;
    }
    m_chatPushFont.setUnderline(m_underlineFontchatPush);
    writeConfig();
}



// принятые сообщения   ----------
void MyClient::on_pb_colorChatPull_clicked()
{
    m_colorChatPull = QColorDialog::getColor(m_colorChatPull, this,
                                             "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorChatPull.isValid())
    {
        debugToInfo("(ChatPull) Цвет: " + m_colorChatPull.name());

        m_temp_colorChatPull = m_colorChatPull;
        m_paletteChatPull.setColor(QPalette::Button, m_colorChatPull);
        ui->pb_colorChatPull->setAutoFillBackground(true);
        ui->pb_colorChatPull->setPalette(m_paletteChatPull);
        ui->pb_colorChatPull->update();
        writeConfig();
    }
    else
    {
        m_colorChatPull = m_temp_colorChatPull;
    }
}

void MyClient::on_cb_chatPullFontSize_activated(int index)
{
    m_chatPullIndexFond = index;
    m_chatPullFont.setPointSize(ui->cb_chatPullFontSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_chatPullFont_clicked()
{
    m_chatPullFont = QFontDialog::getFont(&m_chatPullFontSelected, m_chatPullFont,
                                          this, "Выберети шрифт");

    if (m_chatPullFontSelected)
    {
        if (m_chatPullFont.pointSize() < 10)
        {
            ui->cb_chatPullFontSize->setCurrentIndex(0);
            m_chatPullFont.setPointSize(10);
        }
        else if (m_chatPullFont.pointSize() > 18)
        {
            ui->cb_chatPullFontSize->setCurrentIndex(8);
            m_chatPullFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_chatPullFontSize->findText(QString::number(m_chatPullFont.pointSize()),
                                                   Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_chatPullFontSize->setCurrentIndex(0);
                m_chatPullFont.setPointSize(10);
            }
            else
            {
                ui->cb_chatPullFontSize->setCurrentIndex(value);
                m_chatPullIndexFond = value;
                ui->pb_chatPullFont->setText(m_chatPullFont.family());
            }
        }

        if (m_chatPullFont.weight() > 50)
        {
            m_weightFontchatPull = true;
        }
        else
        {
            m_weightFontchatPull = false;
        }

        if (m_chatPullFont.italic())
        {
            m_italicFontchatPull = true;
        }
        else
        {
            m_italicFontchatPull = false;
        }

        if (m_chatPullFont.underline())
        {
            m_underlineFontchatPull = true;
        }
        else
        {
            m_underlineFontchatPull = false;
        }

        ui->cb_weightFontChatPull->setChecked(m_weightFontchatPull);
        ui->cb_italicFontChatPull->setChecked(m_italicFontchatPull);
        ui->cb_underlineFontChatPull->setChecked(m_underlineFontchatPull);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontChatPull_clicked()
{
    if (ui->cb_weightFontChatPull->isChecked())
    {
        m_weightFontchatPull = true;
    }
    else
    {
        m_weightFontchatPull = false;
    }
    m_chatPullFont.setBold(m_weightFontchatPull);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontChatPull_clicked()
{
    if (ui->cb_italicFontChatPull->isChecked())
    {
        m_italicFontchatPull = true;
    }
    else
    {
        m_italicFontchatPull = false;
    }
    m_chatPullFont.setItalic(m_italicFontchatPull);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontChatPull_clicked()
{
    if (ui->cb_underlineFontChatPull->isChecked())
    {
        m_underlineFontchatPull = true;
    }
    else
    {
        m_underlineFontchatPull = false;
    }
    m_chatPullFont.setUnderline(m_underlineFontchatPull);
    writeConfig();
}

// личные сообщения   ----------------- Private
void MyClient::on_pb_colorChatPrivate_clicked()
{
    m_colorChatPrivate = QColorDialog::getColor(m_colorChatPrivate, this,
                                             "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorChatPrivate.isValid())
    {
        debugToInfo("(ChatPull) Цвет: " + m_colorChatPrivate.name());

        m_temp_colorChatPrivate = m_colorChatPrivate;
        m_paletteChatPrivate.setColor(QPalette::Button, m_colorChatPrivate);
        ui->pb_colorChatPrivate->setAutoFillBackground(true);
        ui->pb_colorChatPrivate->setPalette(m_paletteChatPrivate);
        ui->pb_colorChatPrivate->update();
        writeConfig();
    }
    else
    {
        m_colorChatPrivate = m_temp_colorChatPrivate;
    }
}

void MyClient::on_cb_chatPrivateFontSize_activated(int index)
{
    m_chatPrivateIndexFond = index;
    m_chatPrivateFont.setPointSize(ui->cb_chatPrivateFontSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_chatPrivateFont_clicked()
{
    m_chatPrivateFont = QFontDialog::getFont(&m_chatPrivateFontSelected, m_chatPrivateFont,
                                          this, "Выберети шрифт");

    if (m_chatPrivateFontSelected)
    {
        if (m_chatPrivateFont.pointSize() < 10)
        {
            ui->cb_chatPrivateFontSize->setCurrentIndex(0);
            m_chatPrivateFont.setPointSize(10);
        }
        else if (m_chatPrivateFont.pointSize() > 18)
        {
            ui->cb_chatPrivateFontSize->setCurrentIndex(8);
            m_chatPrivateFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_chatPrivateFontSize->findText(QString::number(
                                                             m_chatPrivateFont.pointSize()),
                                                   Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_chatPrivateFontSize->setCurrentIndex(0);
                m_chatPrivateFont.setPointSize(10);
            }
            else
            {
                ui->cb_chatPrivateFontSize->setCurrentIndex(value);
                m_chatPrivateIndexFond = value;
                ui->pb_chatPrivateFont->setText(m_chatPrivateFont.family());
            }
        }

        if (m_chatPrivateFont.weight() > 50)
        {
            m_weightFontchatPrivate = true;
        }
        else
        {
            m_weightFontchatPrivate = false;
        }

        if (m_chatPrivateFont.italic())
        {
            m_italicFontchatPrivate = true;
        }
        else
        {
            m_italicFontchatPrivate = false;
        }

        if (m_chatPrivateFont.underline())
        {
            m_underlineFontchatPrivate = true;
        }
        else
        {
            m_underlineFontchatPrivate = false;
        }

        ui->cb_weightFontChatPrivate->setChecked(m_weightFontchatPrivate);
        ui->cb_italicFontChatPrivate->setChecked(m_italicFontchatPrivate);
        ui->cb_underlineFontChatPrivate->setChecked(m_underlineFontchatPrivate);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontChatPrivate_clicked()
{
    if (ui->cb_weightFontChatPrivate->isChecked())
    {
        m_weightFontchatPrivate = true;
    }
    else
    {
        m_weightFontchatPrivate = false;
    }
    m_chatPrivateFont.setBold(m_weightFontchatPrivate);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontChatPrivate_clicked()
{
    if (ui->cb_italicFontChatPrivate->isChecked())
    {
        m_italicFontchatPrivate = true;
    }
    else
    {
        m_italicFontchatPrivate = false;
    }
    m_chatPrivateFont.setItalic(m_italicFontchatPrivate);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontChatPrivate_clicked()
{
    if (ui->cb_underlineFontChatPrivate->isChecked())
    {
        m_underlineFontchatPrivate = true;
    }
    else
    {
        m_underlineFontchatPrivate = false;
    }
    m_chatPrivateFont.setUnderline(m_underlineFontchatPrivate);
    writeConfig();
}

// чат группы   -------------------------- Group
void MyClient::on_pb_colorChatGroup_clicked()
{
    m_colorChatGroup = QColorDialog::getColor(m_colorChatGroup, this,
                                             "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorChatGroup.isValid())
    {
        debugToInfo("(ChatPull) Цвет: " + m_colorChatGroup.name());

        m_temp_colorChatGroup = m_colorChatGroup;
        m_paletteChatGroup.setColor(QPalette::Button, m_colorChatGroup);
        ui->pb_colorChatGroup->setAutoFillBackground(true);
        ui->pb_colorChatGroup->setPalette(m_paletteChatGroup);
        ui->pb_colorChatGroup->update();
        writeConfig();
    }
    else
    {
        m_colorChatGroup = m_temp_colorChatGroup;
    }
}

void MyClient::on_cb_chatGroupFontSize_activated(int index)
{
    m_chatGroupIndexFond = index;
    m_chatGroupFont.setPointSize(ui->cb_chatGroupFontSize->currentText().toInt());
    writeConfig();
}

void MyClient::on_pb_chatGroupFont_clicked()
{
    m_chatGroupFont = QFontDialog::getFont(&m_chatGroupFontSelected, m_chatGroupFont,
                                          this, "Выберети шрифт");

    if (m_chatGroupFontSelected)
    {
        if (m_chatGroupFont.pointSize() < 10)
        {
            ui->cb_chatGroupFontSize->setCurrentIndex(0);
            m_chatGroupFont.setPointSize(10);
        }
        else if (m_chatGroupFont.pointSize() > 18)
        {
            ui->cb_chatGroupFontSize->setCurrentIndex(8);
            m_chatGroupFont.setPointSize(18);
        }
        else
        {
            int value{};
            value = ui->cb_chatGroupFontSize->findText(QString::number(
                                                             m_chatGroupFont.pointSize()),
                                                   Qt::MatchExactly | Qt::MatchFixedString);

            if (value == -1)
            {
                debugToInfo("ОШИБКА!!! Не найден размер шрифта", 2);
                ui->cb_chatGroupFontSize->setCurrentIndex(0);
                m_chatGroupFont.setPointSize(10);
            }
            else
            {
                ui->cb_chatGroupFontSize->setCurrentIndex(value);
                m_chatGroupIndexFond = value;
                ui->pb_chatGroupFont->setText(m_chatGroupFont.family());
            }
        }

        if (m_chatGroupFont.weight() > 50)
        {
            m_weightFontchatGroup = true;
        }
        else
        {
            m_weightFontchatGroup = false;
        }

        if (m_chatGroupFont.italic())
        {
            m_italicFontchatGroup = true;
        }
        else
        {
            m_italicFontchatGroup = false;
        }

        if (m_chatGroupFont.underline())
        {
            m_underlineFontchatGroup = true;
        }
        else
        {
            m_underlineFontchatGroup = false;
        }

        ui->cb_weightFontChatGroup->setChecked(m_weightFontchatGroup);
        ui->cb_italicFontChatGroup->setChecked(m_italicFontchatGroup);
        ui->cb_underlineFontChatGroup->setChecked(m_underlineFontchatGroup);

        writeConfig();
    }
}

// жирность шрифта
void MyClient::on_cb_weightFontChatGroup_clicked()
{
    if (ui->cb_weightFontChatGroup->isChecked())
    {
        m_weightFontchatGroup = true;
    }
    else
    {
        m_weightFontchatGroup = false;
    }
    m_chatGroupFont.setBold(m_weightFontchatGroup);
    writeConfig();
}

// курсивный шрифт
void MyClient::on_cb_italicFontChatGroup_clicked()
{
    if (ui->cb_italicFontChatGroup->isChecked())
    {
        m_italicFontchatGroup = true;
    }
    else
    {
        m_italicFontchatGroup = false;
    }
    m_chatGroupFont.setItalic(m_italicFontchatGroup);
    writeConfig();
}

// черта
void MyClient::on_cb_underlineFontChatGroup_clicked()
{
    if (ui->cb_underlineFontChatGroup->isChecked())
    {
        m_underlineFontchatGroup = true;
    }
    else
    {
        m_underlineFontchatGroup = false;
    }
    m_chatGroupFont.setUnderline(m_underlineFontchatGroup);
    writeConfig();
}
// ----------------------------------------------------------------



// ---------------------------------------------------------------
// цвет фона

// -----    общий чат
void MyClient::on_pb_colorFonIndo_clicked()
{
    m_colorFonFormInfo = QColorDialog::getColor(m_colorFonFormInfo, this,
                                          "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorFonFormInfo.isValid())
    {
        debugToInfo("(фон общий чат) Цвет: " + m_colorFonFormInfo.name());

        ui->te_info->setStyleSheet(QString("background-color: %1").arg(m_colorFonFormInfo.name()));
        m_temp_colorFonFormInfo = m_colorFonFormInfo;
        m_paletteFonFormInfo.setColor(QPalette::Button, m_colorFonFormInfo);
        ui->pb_colorFonIndo->setAutoFillBackground(true);
        ui->pb_colorFonIndo->setPalette(m_paletteFonFormInfo);
        ui->pb_colorFonIndo->update();
        writeConfig();
    }
    else
    {
        m_colorFonFormInfo = m_temp_colorFonFormInfo;
    }
}

// ------   цвет для чата личное
void MyClient::on_pb_colorFonPrivat_clicked()
{
    m_colorFonFormPrivat = QColorDialog::getColor(m_colorFonFormPrivat, this,
                                                  "Выберете цвет", QColorDialog::ShowAlphaChannel);

    if (m_colorFonFormInfo.isValid())
    {
        debugToInfo("(фон личный чат) Цвет: " + m_colorFonFormPrivat.name());

        ui->te_privatChat->setStyleSheet(QString("background-color: %1").
                                         arg(m_colorFonFormPrivat.name()));
        m_temp_colorFonFormPrivat = m_colorFonFormPrivat;
        m_paletteFonFormPrivat.setColor(QPalette::Button, m_colorFonFormPrivat);
        ui->pb_colorFonPrivat->setPalette(m_paletteFonFormPrivat);
        ui->pb_colorFonPrivat->setAutoFillBackground(true);
        ui->pb_colorFonPrivat->update();
        writeConfig();
    }
    else
    {
        m_colorFonFormPrivat = m_temp_colorFonFormPrivat;
    }
}

// -----    цвет для чата группы
void MyClient::on_pb_colorFonGroup_clicked()
{
    m_colorFonFormGroup = QColorDialog::getColor(m_colorFonFormGroup, this,
                                                 "Выберете цвет", QColorDialog::ShowAlphaChannel);

   if (m_colorFonFormInfo.isValid())
   {
       debugToInfo("(фон группа чат) Цвет: " + m_colorFonFormGroup.name());

       ui->te_groupChat->setStyleSheet(QString("background-color: %1").
                                       arg(m_colorFonFormGroup.name()));
       m_temp_colorSystemInfo = m_colorFonFormGroup;
       m_paletteSystemInfo.setColor(QPalette::Button, m_colorFonFormGroup);
       ui->pb_colorFonGroup->setPalette(m_paletteSystemInfo);
       ui->pb_colorFonGroup->setAutoFillBackground(true);
       ui->pb_colorFonGroup->update();
       writeConfig();
   }
   else
   {
       m_colorFonFormGroup = m_temp_colorFonFormGroup;
   }
}
// ---------------------------------------------------------------------



// ---------------------------------------------------------------------
// кнопки для чата

// ------------- кнопка очистить чат
void MyClient::on_pb_clearChat_clicked()
{
    if (m_infoWinChat)  // общий чат
    {
        ui->te_info->clear();
    }
    else if (m_privatWinChat)   // личные сообщения
    {
        ui->te_privatChat->clear();
    }
    else if (m_groupWinChat)    // групповой чат
    {
        ui->te_groupChat->clear();
    }
}

// вывод кнопки очистить чат
void MyClient::on_cb_clearText_clicked()
{
    m_clearChat = ui->cb_clearText->isChecked();
    ui->pb_clearChat->setVisible(m_clearChat);
    writeConfig();
}

// ---------    кнопка копировать
void MyClient::on_pb_cutChat_clicked()
{
    if (m_infoWinChat)  // общий чат
    {
        ui->te_info->selectAll();
        ui->te_info->copy();
        ui->te_info->insertPlainText(ui->te_info->toPlainText());
    }
    else if (m_privatWinChat)   // личные сообщения
    {
        ui->te_privatChat->selectAll();
        ui->te_privatChat->copy();
        ui->te_privatChat->insertPlainText(ui->te_privatChat->toPlainText());
    }
    else if (m_groupWinChat)    // групповой чат
    {
        ui->te_groupChat->selectAll();
        ui->te_groupChat->copy();
        ui->te_groupChat->insertPlainText(ui->te_groupChat->toPlainText());
    }
}

// вывод кнопки копировать
void MyClient::on_cb_copuText_clicked()
{
    m_copyChat = ui->cb_copuText->isChecked();
    ui->pb_cutChat->setVisible(m_copyChat);
    writeConfig();
}

// ---------     кнопка сохранить копию чата в файле
void MyClient::on_pb_saveFileChat_clicked()
{
    pathSaveChatFile = QFileDialog::getExistingDirectory(this, "Сохранения файла",
                                                                 pathSaveChatFile);
    debugToInfo("Путь для сохранения файла: " + pathSaveChatFile);

    if (pathSaveChatFile != "")
    {
        if (QFile::exists(pathSaveChatFile + "/save_chat.txt"))
        {
            int i = 1;
            for (; ; ++i)
            {
                if (!QFile::exists(pathSaveChatFile + "/save_chat_" + QString::number(i) + ".txt"))
                {
                    pathSaveChatFile += "/save_chat_" + QString::number(i) + ".txt";
                    break;
                }
            }
        }
        else
        {
            pathSaveChatFile += "/save_chat.txt";
        }

        QFile file(pathSaveChatFile);

        if (file.open(QIODevice::WriteOnly))
        {
            QTextStream out(&file);

            if (m_infoWinChat)  // общий чат
            {
                out << ui->te_info->toPlainText();
            }
            else if (m_privatWinChat)   // личные сообщения
            {
                out << ui->te_privatChat->toPlainText();
            }
            else if (m_groupWinChat)    // групповой чат
            {
                out << ui->te_groupChat->toPlainText();
            }

            file.close();
            systemChatInfo("Файл сохранён");
            debugToInfo("Файл: " + pathSaveChatFile + " сохранён");
        }
        else
        {
            debugToInfo("Ошибка! Не удалось открыть файл: " + pathSaveChatFile + " для записи", 2);
        }
    }
}

// вывод кнопки сохранить чат в файле
void MyClient::on_cb_saveFileChat_clicked()
{
    m_saveFileChat = ui->cb_saveFileChat->isChecked();
    ui->pb_saveFileChat->setVisible(m_saveFileChat);
    writeConfig();
}
// ---------------------------------------------------------------------


// --------------------------------------------------------
// дублировать debug сообщения в другие чаты
void MyClient::on_cb_debugCopyForChat_clicked()
{
    m_copyDebugForChat = ui->cb_debugCopyForChat->isChecked();
    writeConfig();
}

// дублировать системные сообщения в другие чаты
void MyClient::on_cb_systemCopyForChat_clicked()
{
    m_copySystemForChat = ui->cb_systemCopyForChat->isChecked();
    writeConfig();
}
// ---------------------------------------------------------
