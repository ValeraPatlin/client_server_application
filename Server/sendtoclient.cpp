#include "myserver.h"
#include "ui_myserver.h"



void MyServer::sendToClient(QTcpSocket *pSocket,
                            Tranciver_commmand command, int number, QString str,
                            PackegeDate packege_date)
{
    qDebug() << "\n ----------------------------- \n"
             << Q_FUNC_INFO
             << "\n  command:  " << command
             << "\n  num: " << number
             << "\n  str: " << str;

//    ui->te_info->append("\n отправка: \n");
//    ui->te_info->append("command - " + QString::number(command));
//    ui->te_info->append("number - " + QString::number(number));
//    ui->te_info->append("str - " + str);

    if (command == 0)
    {
        ui->te_info->append("\n\n  \t\t  ERROR \n пришла команда: 0 \n\n");
        return;
    }


    try
    {
        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        out << quint16(0) << command;

        if (number != -1)
        {
            out << number;
        }

        if (str != "")
        {
            out << str;
        }

        // список клинетов
        if (command == comListClient)
        {
            out << m_playerList;
        }

//        switch (command)
//        {
//            case comRegistratiomYes:
//            case comPairFound:
//            case comCountPlayer:
//            case comOpponentOffer:
//            case comClientLeaveGroupsKNB:
//            case comToStepKNB:
//            case comMotionStartTimerKNB:
//            case comDefeatKNB:
//            {
//                // out << number;
//                break;
//            }

//            case comMessagePull:
//            case comOfferToJoinKNB:
//            case comCloseInvitation:
//            {
//                // out << number << str;
//                break;
//            }

//            case comListClient:
//            {
//                out << m_playerList;
//                break;
//            }
//        }

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        pSocket->write(arrBlock);
    }
    catch (...)
    {
        qDebug() << "ERROR!!!";
    }
    qDebug() << "-------------------------------- \n";
}



void MyServer::sendToClient(QTcpSocket *pSocket,
                            Tranciver_commmand command,
                            QVector<int> &vec, QString str, int host)
{
    qDebug() << "\n ----------------------------- \n"
             << Q_FUNC_INFO
             << "\n  vec: " << vec.size();

    try
    {
        QByteArray arrBlock;
        QDataStream out(&arrBlock, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);

        out << quint16(0) << command; // << str << vec;

        if (str != "")
        {
            out << str;
        }

//        QVector<int> v;
//        for (auto elem : vec)
//        {
//            v.push_back(elem);
//        }

        if (vec.size() != 0)
        {
            out << vec;
            //out << vec;
//            out << vec.size();
//            for (auto elem : vec)
//            {
//                out << elem;
//            }
        }

        if (host != -1)
        {
            out << host;
        }

        out.device()->seek(0);
        out << quint16(arrBlock.size() - sizeof(quint16));
        pSocket->write(arrBlock);
    }
    catch (...)
    {
        qDebug() << "ERROR!!!";
    }
    qDebug() << "-------------------------------- \n";
}






