#include "myserver.h"
#include "ui_myserver.h"



MyServer::MyServer(int nPort, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyServer)
{
    ui->setupUi(this);

    m_ptcpServer = new QTcpServer(this);

    if (!m_ptcpServer->listen(QHostAddress::Any, nPort))
    {
        ui->te_info->setText("Server Error!!!  \n");
    }

    connect(m_ptcpServer, SIGNAL(newConnection()), this, SLOT(slotNewConnection()));

    ui->te_info->setReadOnly(true);


    // база данных
    m_databaseGood = false;
}

MyServer::~MyServer()
{
    delete ui;
}


void MyServer::slotNewConnection()
{
    QTcpSocket* pClientSocket = m_ptcpServer->nextPendingConnection();

    connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(slotDeleteClient()));

    //connect(pClientSocket, SIGNAL(disconnected()), pClientSocket, SLOT(deleteLater()));

    connect(pClientSocket, SIGNAL(readyRead()), this, SLOT(slotReadClient()));
}



// --------------------------------------------------------------------------
// подбор пары
std::tuple<Player, bool> MyServer::creatingPair(int id)
{
    qDebug() << Q_FUNC_INFO
             << "\n count:  " << m_playersMap.count()
             << "\n id: " << id;



    if (m_playersMap.count() > 1 && id > 0)
    {
        for (auto plauyerOpponent = m_playersMap.begin();
             plauyerOpponent != m_playersMap.end(); ++plauyerOpponent)
        {
//            qDebug() << plauyerOpponent.key() << plauyerOpponent.value().idOpponent;

//            if (plauyerOpponent.key() != id && plauyerOpponent.value().idOpponent == -1)
//            {
//                plauyerOpponent.value().idOpponent = id;

//                auto mainPlayer = m_playersMap.find(id);
//                mainPlayer.value().idOpponent = plauyerOpponent.value().id;

//                //Player p = mainPlayer.value();

//                qDebug() << "Игрок #1: " << plauyerOpponent.value().idOpponent
//                         << "Игрок #2: " << mainPlayer.value().idOpponent;

//                ui->te_info->append("Пара: \n Игрок #1: " + QString::number(plauyerOpponent.key())
//                                    + " против: " + QString::number(plauyerOpponent.value().idOpponent)
//                                    + "\n Игрок #2: " + QString::number(mainPlayer.key())
//                                    + " против: " + QString::number(mainPlayer.value().idOpponent));

//                return std::make_tuple(plauyerOpponent.value(), true);
//            }
        }
    }
    Player p;
    return std::make_tuple(p, false);
}
// ---------------------------------------------------------------------------


// --------------------------------------------------------------------------
// создание уникального id
int MyServer::registrationServer()
{
    for (int i = 0; i < 10; ++i)
    {
        int idClient = QRandomGenerator::global()->bounded(1, 1100);
        auto it = m_playersMap.find(idClient);
        if (it == m_playersMap.end())
        {
            return idClient;
        }
    }
    return -1;
}
// -------------------------------------------------------------------------


// -------------------------------------------------------------------------
// количество играков
void MyServer::countPlayer()
{
    for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
    {
        if (it.value().pClientSocket)
        {
            sendToClient(it.value().pClientSocket, comCountPlayer, m_playersMap.count());
        }
        else
        {
            m_playersMap.remove(it.key());
        }
    }
}

// список с клиентами, их статусом и во что играют
void MyServer::listClient()
{
    m_playerList.clear();

    for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
    {
        m_playerList << QString(QString::number(it.value().id) + " \t "
                                + QString(it.value().name == "" ? "аноним" : it.value().name) + " \t "
                                + it.value().status + " \t "
                                + QString(it.value().generalChat ? "чат открыт" : "чат закрыт")
                                + " \t"
                                + QString(it.value().gameKNB ? "играет в КНБ" : ""));

        qDebug() << m_playerList;
    }

    for (auto it = m_playersMap.begin(); it != m_playersMap.end(); ++it)
    {
        sendToClient(it.value().pClientSocket, comListClient);
    }
}

void MyServer::writeStatusClient(int id, int status)
{
    qDebug() << "status client chat:  " << status;

    auto mainPlayer = m_playersMap.find(id);

    if (status != -1)
    {
        mainPlayer.value().status =
                (status == 0) ? "готов играть"
                                    : (status == 1) ? "только чат \t" : "занят \t";

        qDebug() << mainPlayer.value().status;
    }

    listClient();
}
// -------------------------------------------------------------------------






void MyServer::slotDeleteClient()
{
    qDebug() << "\n\n !!! \n\n";
}
