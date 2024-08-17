#include "myclient.h"
#include "ui_myclient.h"


// ------------------------------------------------------------------
// кнопка зарегистрироваться анонимно
void MyClient::on_pb_anonymous_clicked()
{
    debugToInfo("Вызван метод: on_pb_anonymous_clicked");
    ui->lb_infoReg->setVisible(false);
    connectToServer();
}

// регистрация на сервере с логином и раролем
void MyClient::on_pb_registServer_clicked()
{
    // нужна база данных
    debugToInfo("Вызван метод: on_pb_registServer_clicked");
    connectToServer(true);
}

// авторизация на сервере
void MyClient::on_pb_log_in_clicked()
{
    debugToInfo("Вызван метод: on_pb_log_in_clicked");

    if (offlineMode)    // офлайн режим
    {
        enabledGameForm();
        systemChatInfo("Вы в offline режиме. В этом режиме чат недоступен");
        ui->pb_log_in->setText("Вы в offline режиме");
        ui->pb_log_in->setEnabled(false);
        ui->hs_countPlayerKNB->setEnabled(false);
        ui->sb_countPlayerKNB->setEnabled(false);
        ui->lb_infoReg->setVisible(false);

        on_rb_totalChat_clicked();
        ui->rb_totalChat->setChecked(true);
    }
    else if (onlineFlag)    // отключится от сервера
    {
        sendToServer(comCloseClient);
        enabledFieldForm();
        statusChatPrivate(false);   // статус чата личных сообщений
        ui->pb_log_in->setText("Авторизироваться");
        ui->cb_offline->setEnabled(true);
        ui->lb_myId->setText("Нет данных");
        ui->lb_countPlayer->setText("Нет данных");

        ui->tab_listClientAndGroup->setTabEnabled(1, false);    // вкладка группы

        chatBlok(false);     // блокировка чата
        ui->pb_offerClientGroup->setEnabled(false);
        ui->b_nameOfferClientGroup->setEnabled(false);
        enabledGameForm(false);
        visibleGameKNB(false);
        onlineFlag = false;
    }
    else    // подключится к серверу
    {
        if (ui->le_login->text() != "" && ui->le_passwd->text() != "")
        {
            if (ui->cb_loginSave->isChecked())
            {
                strLogun = ui->le_login->text();
                if (ui->cb_passwdSave->isChecked())
                {
                    strPasswd = ui->le_passwd->text();
                }
                writeConfig();
            }

            ui->hs_countPlayerKNB->setEnabled(true);
            ui->sb_countPlayerKNB->setEnabled(true);
            ui->lb_infoReg->setVisible(false);

            connectToServer(true);
        }
        else
        {
            ui->lb_infoReg->setVisible(true);
            ui->lb_infoReg->setText("Укажите логин и пароль!");
        }
    }

    // для зашиты от спама
    if (m_blokConnectServer >= 3)
    {
        systemChatInfo("Кнопки подключения заблокированны");
        debugToInfo("Кнопки подключения заблокированны", 3);
        m_blokConnectServer = 0;
        m_blokTimer.start();
        ui->pb_anonymous->setEnabled(false);
        ui->pb_log_in->setEnabled(false);
        ui->cb_offline->setEnabled(false);
        ui->lb_infoReg->setVisible(true);
        ui->lb_infoReg->setText("Кнопки подключения заблокированны на: "
                                + QString:: number(m_stepBlokTimer));
    }
}

// подключение
void MyClient::connectToServer(bool reg)
{
    debugToInfo("Вызван метод:  connectToServer");
    debugToInfo("Флаг reg = " + QString::number(reg));

    if (ui->le_ipAddress->text() != "" && ui->le_port->text() != "")
    {
        m_pTcpSocket->connectToHost(ui->le_ipAddress->text(), ui->le_port->text().toInt());

        if (!m_pTcpSocket->waitForConnected())
        {
            debugToInfo("Сервер недоступен!", 1);
        }
        else
        {
            if (reg)
            {
                sendToServer(comConnectUser);
            }
            else
            {
                sendToServer(comRegistration);
            }
        }
    }
    else
    {
        ui->lb_infoReg->setVisible(true);
        ui->lb_infoReg->setText("Укажите ip адрес и порт для подключения!");
    }
}


// офлайн режим
void MyClient::on_cb_offline_clicked()
{
    if (ui->cb_offline->isChecked())
    {
        ui->pb_log_in->setText("Играть офлайн");
        offlineMode = true;
    }
    else
    {
        ui->pb_log_in->setText("Авторизироваться");
        offlineMode = false;
        enabledGameForm(false);
        ui->pb_log_in->setEnabled(true);
    }

    debugToInfo("Флаг офлйна: " + QString::number(ui->cb_offline->isChecked()));

    ui->pb_gameStartKNB->setEnabled(offlineMode);   // кнопка играть
    ui->gb_offlineStatus->setVisible(offlineMode);  // поле для статуса хода компа
    ui->gb_statusOnlineKNB->setVisible(!offlineMode);   // сокрытие данных для поиска группы
    infoMotionFormKNB("");

    ui->lb_infoReg->setVisible(false);
    enabledFieldForm(!ui->cb_offline->isChecked());
}
// ----------------------------------------------------------------------



// -------------------------------------------------------------
// таймер блокировки подлкючения (защита от спама)
void MyClient::slotBlokConnectServer()
{
    if (m_stepBlokTimer == 0)
    {
        m_blokTimer.stop();
        m_stepBlokTimer = 10;
        ui->pb_anonymous->setEnabled(true);
        ui->pb_log_in->setEnabled(true);
        ui->cb_offline->setEnabled(true);
        ui->lb_infoReg->setVisible(false);
    }
    else
    {
        ui->lb_infoReg->setText("Кнопки подключения заблокированны на: "
                                + QString:: number(--m_stepBlokTimer));
    }
}
// -------------------------------------------------------------



// -------------------------------------------------------------
// блокировка полей регестрации при подключении
void MyClient::enabledFieldForm(bool flag)
{
    debugToInfo("Флаг офлайна: " + QString::number(flag));

    ui->pb_anonymous->setVisible(flag);
    ui->cb_loginSave->setEnabled(flag);
    ui->cb_passwdSave->setEnabled(flag);
    ui->le_login->setEnabled(flag);
    ui->le_passwd->setEnabled(flag);
    ui->pb_registServer->setEnabled(flag);
    ui->gameListWidget->clear();
    ui->gameListWidget->setEnabled(!flag);
}

// блокировка вкладки игры
void MyClient::enabledGameForm(bool flag)
{
    ui->tab_gameField->setTabEnabled(1, flag);
    ui->tab_gameField->setTabEnabled(2, flag);
}
// --------------------------------------------------------------



void MyClient::slotError()
{
    QString strError = "ERROR:  " + (QString(m_pTcpSocket->errorString()));

    debugToInfo(strError, 1);
}


void MyClient::slotConnected()
{
    debugToInfo("connect to server", 3);
}

