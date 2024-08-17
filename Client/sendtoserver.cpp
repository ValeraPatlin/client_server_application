#include "myclient.h"
#include "ui_myclient.h"


// отправка данных на сервер основной метод
void MyClient::sendToServer(Tranciver_commmand command, int idOpponent, QString gameOffer)
{
    debugToInfo("Вызван метод: sendToServer");

    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);
    out << quint16(0) << command << idClient;   // клиент всегда посылает команду и id

    debugToInfo("Вызванна команда: " + QString::number(command));

    switch (command)
    {
        case comRegistration:
        {
            out << m_statusClient;
            out << ui->cb_totalChat->isChecked();
            out << ui->cb_privatChat->isChecked();
            break;
        }

        case comGameStart:
        {
            break;
        }

        // --------------------------------------------------
        // KNB
        case comSelectionPlayerKNB:
        {
            debugToInfo("comSelectionPlayerKNB");
            debugToInfo("Передано количество участников: "
                        + QString::number(ui->hs_countPlayerKNB->value()));
            out << ui->hs_countPlayerKNB->value();
            out << gameOffer;
            break;
        }

        case comMotionKNB:
        {
            out << gameOffer;
            break;
        }
        case comOfferToJoinTake:
        {
            out << idOpponent << gameOffer;
            break;
        }
        // --------------------------------------------------

        case comToStepKNB:
        {
            out << m_step;
            break;
        }


        case comConnectUser:
        {
             out << ui->le_login->text();
             out << ui->le_passwd->text();
             break;
        }


        // -------------------------------------------------
        // чат
        case comGeneralChatOnOff:
        {
            out << ui->cb_totalChat->isChecked();
            break;
        }

        case comPrivateChatOnOff:
        {
            out << ui->cb_privatChat->isChecked();
            break;
        }

        case comMessagePush:
        {
            out << ui->le_message->text();
            //out << ui->cb_totalChat->isChecked();
            break;
        }

        case comMessagePushPrivat:
        {
            out << idOpponent << ui->le_message->text();
            break;
        }

        case comMessagePushGroup:
        {
            out << ui->cb_nameChatGroup->currentText() << ui->le_message->text();
            break;
        }

        case comStatusClientChat:
        {
            out << m_statusClient;
            break;
        }
        // -----------------------------------------------


        // -----------------------------------------------
        // группа
        case comOfferGroup:
        case comDelClientGroup:
        {
            out << m_listClient.at(0) << ui->b_nameOfferClientGroup->currentText();
            break;
        }

        case comDelTotalGroup:
        case comLeaveClientGroup:
        {
            out << ui->b_nameOfferClientGroup->currentText();
            break;
        }
    }

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
}



// отправка данных на сервер
void MyClient::sendDataToServer(Tranciver_commmand command, const QVariantList &data)
{
    QByteArray arrBlock;
    QDataStream out(&arrBlock, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_11);

    out << quint16(0) << command << data;

    out.device()->seek(0);
    out << quint16(arrBlock.size() - sizeof(quint16));
    m_pTcpSocket->write(arrBlock);
}



