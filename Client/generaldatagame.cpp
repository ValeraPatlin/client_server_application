#include "myclient.h"
#include "ui_myclient.h"



// --------------------------------------------------------------
// принять предложение поиграть в KNB
void MyClient::on_pb_acceptOne_clicked()
{
    sendToServer(comOfferToJoinTake, idOpponent_1, gameOffer_1);
    idOpponent_1 = -1;
    gameOffer_1.clear();
    ui->gb_game_1->setVisible(false);

    if (m_findStartKNB)
    {
        sendToServer(comStopFindGroupsKNB);
    }
}

void MyClient::on_pb_acceptTwo_clicked()
{
    sendToServer(comOfferToJoinTake, idOpponent_2, gameOffer_2);
    idOpponent_2 = -1;
    gameOffer_2.clear();
    ui->gb_game_2->setVisible(false);

    if (m_findStartKNB)
    {
        sendToServer(comStopFindGroupsKNB);
    }
}

void MyClient::on_pb_acceptThree_clicked()
{
    sendToServer(comOfferToJoinTake, idOpponent_3, gameOffer_3);
    idOpponent_3 = -1;
    gameOffer_3.clear();
    ui->gb_game_3->setVisible(false);

    if (m_findStartKNB)
    {
        sendToServer(comStopFindGroupsKNB);
    }
}

// --------------------------
// отказаться от игры
void MyClient::on_pb_formOneRejoction_clicked()
{
    sendToServer(comOfferRejoction);

    idOpponent_1 = -1;
    gameOffer_1.clear();

    ui->gb_game_1->setVisible(false);
}

void MyClient::on_pb_formTwoRejoction_clicked()
{
    sendToServer(comOfferRejoction);

    idOpponent_2 = -1;
    gameOffer_2.clear();

    ui->gb_game_2->setVisible(false);
}

void MyClient::on_pb_formThreeRejoction_clicked()
{
    sendToServer(comOfferRejoction);

    idOpponent_3 = -1;
    gameOffer_3.clear();

    ui->gb_game_3->setVisible(false);
}




// убрать одинаковые приглашения
void MyClient::closeGameOffer(QString str)
{
    if (gameOffer_1 == str)
    {
        ui->gb_game_1->setVisible(false);
        idOpponent_1 = -1;
        gameOffer_1.clear();
    }
    if (gameOffer_2 == str)
    {
        ui->gb_game_2->setVisible(false);
        idOpponent_2 = -1;
        gameOffer_2.clear();
    }
    if (gameOffer_3 == str)
    {
        ui->gb_game_3->setVisible(false);
        idOpponent_3 = -1;
        gameOffer_3.clear();
    }
}

void MyClient::offerToJoin(int idOpp, QString game, bool grop)
{
    if (idOpponent_1 == -1)
    {
        idOpponent_1 = idOpp;
        gameOffer_1 = game;
        ui->gb_game_1->setVisible(true);
        ui->lb_box_1->setText("Id: " + QString::number(idOpponent_1)
                              + QString(grop ? " пригласил вас в группу: \n"
                                             : " пригласил на партию: \n")
                              + gameOffer_1);
    }
    else if (idOpponent_2 == -1)
    {
        idOpponent_2 = idOpp;
        gameOffer_2 = game;
        ui->gb_game_2->setVisible(true);
        ui->lb_box_2->setText("Id: " + QString::number(idOpponent_2)
                              + QString(grop ? " пригласил вас в группу: \n"
                                             : " пригласил на партию: \n")
                              + gameOffer_2);
    }
    else if (idOpponent_3 == -1)
    {
        idOpponent_3 = idOpp;
        gameOffer_3 = game;
        ui->gb_game_3->setVisible(true);
        ui->lb_box_3->setText("Id: " + QString::number(idOpponent_3)
                              + QString(grop ? " пригласил вас в группу: \n"
                                             : " пригласил на партию: \n")
                              + gameOffer_3);
    }
    else
    {
        //
    }
}
// --------------------------------------------------------------


