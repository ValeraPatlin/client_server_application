#include "myclient.h"
#include "ui_myclient.h"



// ----------------------------------------------------------------------
// методы по работе сo списком клиентов
void MyClient::on_gameListWidget_currentTextChanged(const QString &currentText)
{
    // в разработке
    qDebug() << currentText;    // удалить
    // "1016 \t аноним \t готов играть \t чат открыт \t";
    // list =   ("1016", "аноним", "готов играть", "чат открыт \t")


    if (currentText != "")
    {
        m_listClient.clear();
        m_listClient = currentText.split(" \t ");

        // личные сообщения --------------
        if (m_privateclick)     // если чат переключен на приватный
        {
            if (QString::number(idClient) != m_listClient.at(0))
            {
                ui->lb_messageText->setText("Ваше сообщение отправиться в личный чат: id - "
                                            + m_listClient.at(0) + " name - " + m_listClient.at(1));
                ui->gb_mesegChat->setEnabled(true);
            }
            else
            {
                ui->lb_messageText->setText("Вы выбрали собственный клиент из списка.");
                ui->gb_mesegChat->setEnabled(false);
                ui->gb_mesegChat->setToolTip("Вы не можете отправить личное сообщение сами себе.");
            }
        }
        // -------------------------


        // обработка приглашения в группу ------------
        chekClientListGroup();
    }
    else
    {
        // ??????????????  надо ли ?????????????????
    }
}
// ----------------------------------------------------------------------


// обработка приглашения в группу
void MyClient::chekClientListGroup()
{
       // выбрал клиент
    if (m_listClient.size() != 0 && QString::number(idClient) != m_listClient.at(0))
    {
        ui->pb_offerClientGroup->setEnabled(true);
        chekClientGroup();
    }
    else    // выбрал сам себя
    {
        chekMyClientGroup();
    }
}


// исключить, покинуть, пригласить  (основной метод если выбрал клиент)
void MyClient::chekClientGroup()
{
    try
    {
        if (ui->b_nameOfferClientGroup->currentText() == "group#1"
                && chekClientStatus(vecGropu_1, m_hostGroup_1))    // группа 1
        {
            return;
        }
        else if (ui->b_nameOfferClientGroup->currentText() == "group#2"
                 && chekClientStatus(vecGropu_2, m_hostGroup_2))        // группа 2
        {
            return;
        }
        else if (ui->b_nameOfferClientGroup->currentText() == "group#3"
                 && chekClientStatus(vecGropu_3, m_hostGroup_3))      // группа 3
        {
            return;
        }
        else if (ui->b_nameOfferClientGroup->currentText() == "KNB"
                 && chekClientStatus(vecOpponentKNB, m_hostGroup_KNB))      // группа 3
        {
            return;
        }

        if (!m_listClient.empty())
        {
            if ((m_group_1 && ui->b_nameOfferClientGroup->currentText() == "group#1" &&!m_hostGroup_1)
            || (m_group_2 && ui->b_nameOfferClientGroup->currentText() == "group#2" &&!m_hostGroup_2)
            || (m_group_3 && ui->b_nameOfferClientGroup->currentText() == "group#3" &&!m_hostGroup_3))
            {
                ui->pb_offerClientGroup->setEnabled(false);
                ui->pb_offerClientGroup->setText("Вы в этой группе");
                ui->pb_offerClientGroup->setToolTip("Вы не можете пригласить другого клиента "
                                                    "в группу, так как не являетесь host.");
            }
//            else if (m_groupKNB && ui->b_nameOfferClientGroup->currentText() == "KNB")
//            {
//                debugToInfo("\n !!!!!!!!!!!!!!1!!!!! \n ");
//            }
            else
            {
                ui->pb_offerClientGroup->setText("Пригласить в группу:");
                m_statusButtonGroup = statusOffer;
            }
        }
        else    // если список клиентов пуст - то блокируем кнопку
        {
            ui->pb_offerClientGroup->setEnabled(false);
        }
    }
    catch (...)
    {
        debugToInfo("\n Возникла ошибка при попытке изменить название кнопки! \n", 1);
    }
}

// выбрал сам себя
void MyClient::chekMyClientGroup()
{
    if ((m_hostGroup_1 && ui->b_nameOfferClientGroup->currentText() == "group#1")
            || (m_hostGroup_2 && ui->b_nameOfferClientGroup->currentText() == "group#2")
            || (m_hostGroup_3 && ui->b_nameOfferClientGroup->currentText() == "group#3"))
    {
        ui->pb_offerClientGroup->setText("Распустить группу:");
        ui->pb_offerClientGroup->setToolTip("Распустить всю группу");
        ui->pb_offerClientGroup->setEnabled(true);
        m_statusButtonGroup = statusDelGroup;
    }
    else
    {
        if ((m_group_1 && ui->b_nameOfferClientGroup->currentText() == "group#1")
                || (m_group_2 && ui->b_nameOfferClientGroup->currentText() == "group#2")
                || (m_group_3 && ui->b_nameOfferClientGroup->currentText() == "group#3"))
        {
            ui->pb_offerClientGroup->setText("Покинуть группу");
            m_statusButtonGroup = statusLeave;
            ui->pb_offerClientGroup->setEnabled(true);
            ui->pb_offerClientGroup->setToolTip("Прокинуть эту группу.");
        }
        else
        {
            ui->pb_offerClientGroup->setText("Это вы");
            m_statusButtonGroup = statusDefoltGroup;
            ui->pb_offerClientGroup->setEnabled(false);
            ui->pb_offerClientGroup->setToolTip("Вы выбрали собственный клиент из списка.");
        }
    }
}


void MyClient::on_b_nameOfferClientGroup_activated(const QString &arg1)
{
    chekClientListGroup();
}


// для сообщений в чат
void MyClient::on_cb_nameChatGroup_activated(const QString &arg1)
{
    if (ui->cb_nameChatGroup->currentIndex() != 0)
    {
        if ((arg1 == "group#1" && m_group_1)
                || (arg1 == "group#2" && m_group_2)
                || (arg1 == "group#3" && m_group_3)
                || (arg1 == "KNB" && m_groupKNB))
        {
            ui->lb_messageText->setText("Вы пишете в группу: " + arg1);
            ui->gb_mesegChat->setEnabled(true);
        }
        else
        {
            ui->lb_messageText->setText("Вы не состоите в группе: " + arg1);
            ui->gb_mesegChat->setEnabled(false);
        }

//        if (m_privateclick)    // личный чат
//        {
//            m_privateclick = false;
//        }
    }
    else
    {
        if (m_privateclick)    // личный чат
        {
            if (m_listClient.size() != 0 && QString::number(idClient) != m_listClient.at(0))
            {
                ui->lb_messageText->setText("Ваше сообщение отправиться в личный чат: id - "
                                            + m_listClient.at(0) + " name - " + m_listClient.at(1));
                ui->gb_mesegChat->setEnabled(true);
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
}







bool MyClient::chekClientStatus(QVector<int> &vecGroup, bool hostGroup)
{
    if (vecGroup.empty() || m_listClient.empty())   // вектор пуст изменений не произошло
    {
        return false;
    }

    for (auto elem : vecGroup)
    {
        if (QString::number(elem) == m_listClient.at(0))
        {
            if (hostGroup)
            {
                ui->pb_offerClientGroup->setText("Исключить из группу:");
                m_statusButtonGroup = statusDelClient;
            }
            else
            {
                ui->pb_offerClientGroup->setText("Вы в группе:");
                ui->pb_offerClientGroup->setToolTip("Вы выбрали клиента из своей группы.");
                ui->pb_offerClientGroup->setEnabled(false);
                m_statusButtonGroup = statusDefoltGroup;
            }
            return true;
        }
    }
    return false;
}

//void MyClient::textClientLabelGroup(int numGroup)
//{
//    if (numGroup == 1)  // группа 1
//    {
//        textClientLabelGroup(ui->lb_id_groups, vecGropu_1, m_idHost_1, m_hostGroup_1);
//    }
//    else if (numGroup == 2)     // группа 2
//    {
//        textClientLabelGroup(ui->lb_id_groups_2, vecGropu_2, m_idHost_2, m_hostGroup_2);
//    }
//    else if (numGroup == 3)     // группа 3
//    {
//        textClientLabelGroup(ui->lb_id_groups_3, vecGropu_3, m_idHost_3, m_hostGroup_3);
//    }
//    else if (numGroup == 4)     // группа KNB
//    {
//        textClientLabelGroup(ui->lb_id_groups_KNB, vecOpponentKNB, m_idHost_KNB, m_hostGroup_KNB);
//    }
//}

void MyClient::textClientLabelGroup(QLabel *lb_group, QVector<int> vec, int idHost, bool host)
{
    lb_group->setText("Вы (" + QString::number(idClient)
                      + QString(host ? " host" : "")
                      + "),  ");

    for (auto it : vec)
    {
        lb_group->setText(lb_group->text() + "  "
                          + QString::number(it)
                          + QString(idHost == it ? "(host)" : "")
                          + ",");
    }
}

// этот метот надо вызывать как только изменилась переменная m_countGroup
void MyClient::textNameChatGroup()
{
    if (m_countGroup == 0)
    {
        ui->lb_textInfoGroup->setText("Вы не состоите в группе.");
        ui->cb_nameChatGroup->setVisible(false);
        ui->lb_id_groups->setText("Нет данных");
        ui->lb_id_groups_2->setText("Нет данных");
        ui->lb_id_groups_3->setText("Нет данных");
    }
    else
    {
        ui->lb_textInfoGroup->setText("Писать в группу:");
        ui->cb_nameChatGroup->setVisible(true);
    }
}

// очистка данных если клиент покинул все круппы
void MyClient::clearDataClientGroup()
{
    m_hostGroup_1 = false;
    m_hostGroup_2 = false;
    m_hostGroup_3 = false;
    m_group_1 = false;
    m_group_2 = false;
    m_group_3 = false;
    m_idHost_1 = 0;
    m_idHost_2 = 0;
    m_idHost_3 = 0;
    vecGropu_1.clear();
    vecGropu_2.clear();
    vecGropu_3.clear();
}

// в каких группах состоит клиент
void MyClient::setTextLabelGroup(QString group)
{
    QString text = ui->lb_names_groups->text();
    QString text2 = text.simplified();
    text2.remove(",");
    QStringList groupList = text2.split(" ");
    groupList.removeAll(group);
    QString newText = groupList.join(", ");

    if (newText == "")
    {
        ui->lb_names_groups->setText("Нет данных");
    }
    else
    {
        ui->lb_names_groups->setText(newText);
    }
}

// удалить данные о группе
void MyClient::delDataGroup(QString group)
{
    if (group == "group#1")
    {
        vecGropu_1.clear();
        m_group_1 = false;
        m_idHost_1 = 0;
        m_hostGroup_1 = false;
        ui->lb_id_groups->clear();
        ui->lb_id_groups->setText("Нет данных");
    }
    else if (group == "group#2")
    {
        vecGropu_2.clear();
        m_group_2 = false;
        m_idHost_2 = 0;
        m_hostGroup_2 = false;
        ui->lb_id_groups_2->clear();
        ui->lb_id_groups_2->setText("Нет данных");
    }
    else if (group == "group#3")
    {
        vecGropu_3.clear();
        m_group_3 = false;
        m_idHost_3 = 0;
        m_hostGroup_3 = false;
        ui->lb_id_groups_3->clear();
        ui->lb_id_groups_3->setText("Нет данных");
    }
}


void MyClient::leaveClientTotalGroup()
{
    //
}

// основной метод для очистки данных при удаление клинта и группы
void MyClient::chekDelClientGroup(QString group)
{
    setTextLabelGroup(group);

    --m_countGroup;
    textNameChatGroup();

    delDataGroup(group);

    chekClientListGroup();
    //chekClientGroup();
}



// пригласить клиента в группу или в игру
void MyClient::on_pb_offerClientGroup_clicked()
{
    debugToInfo("Выбранно id - " + m_listClient.at(0) + " name - " + m_listClient.at(1)
                + " group - " + ui->b_nameOfferClientGroup->currentText());

    switch (m_statusButtonGroup)
    {
        case statusDefoltGroup:
        {
            debugToInfo("Ошибка! Нулевой статус кнопки приглашения", 2);
            break;
        }

        // пригласить клиента в группу или в игру
        case statusOffer:
        {
            sendToServer(comOfferGroup);
            break;
        }

        // исключить клиента
        case statusDelClient:
        {
            sendToServer(comDelClientGroup);
            break;
        }

        // распустить группу
        case statusDelGroup:
        {
            sendToServer(comDelTotalGroup);
            break;
        }

        // покинуть группу (клиент сам покидает группу)
        case statusLeave:
        {
            sendToServer(comLeaveClientGroup);
            break;
        }

        default:
        {
            debugToInfo("\n Возникла ошибка! \n Статус кнопки пригласить: "
                        + QString::number(m_statusButtonGroup) + "\n", 2);
            break;
        }
    }
}
