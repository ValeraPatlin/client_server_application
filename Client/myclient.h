#ifndef MYCLIENT_H
#define MYCLIENT_H

#include <QWidget>
#include <QComboBox>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QHostInfo>
#include <QString>
#include <QStringList>
#include <QDebug>
#include <QVariantList>
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QSettings>
#include <QListWidgetItem>
#include <QVector>
#include <QTimer>
#include <QDateTime>
#include <QColorDialog>
#include <QColor>
#include <QPalette>
#include <QFont>
#include <QFontDialog>
#include <QMessageBox>
#include <QTextCursor>
#include <QTextCharFormat>
#include <QRandomGenerator>
#include <QIcon>

#include "../Server/tranciver.h"
#include "formregistrationonline.h"


class FormRegistrationOnline;


namespace Ui {
class MyClient;
}

enum StatusFormKNB
{
    statusDefolt = 0,
    stopFindGroupsKNB = 1,
    clientLeaveKNB,
};

// int m_statusButtonGroup = 0;    // 1 - , 2 - , 3 - , 0 -
enum StatusButtonGroup
{
    statusDefoltGroup = 0,      // нет статуса
    statusOffer = 1,            // принять
    statusDelClient = 2,        // исключть
    statusDelGroup = 3,         // распустить
    statusLeave = 4,            // покинуть группу
};

struct RegistrationClient
{
    QString m_logen;
    QString m_passwd;
    QString m_name;
    int m_id{};
};

class MyClient : public QWidget
{
    Q_OBJECT

    QTcpSocket* m_pTcpSocket;
    quint16 m_nNextBlockSize{};

    StatusFormKNB m_statusFormKNB;
    StatusButtonGroup m_statusButtonGroup;

    FormRegistrationOnline regisOnline();

    int idClient{};     // переменная где хранится id клиента
    bool onlineFlag{};      // флаг для обазночения что клиент подключился
    bool offlineMode{};         // флаг для переключения offline online
    QString strIPAddres;
    QString m_tempStrIPAddress;
    int m_port {};
    int m_tempPort{};
    QString strLogun;
    QString strPasswd;
    bool m_totalChat{};         // общий чат
    bool m_privatChat{};        // личный чат
    bool m_infoWinChat{};       // переключение на общий чат
    bool m_privatWinChat{};     // переключение на личный чат
    bool m_groupWinChat{};      // переключение на чат группы
    bool m_messagePrivateChat{};    // показывать личные сообщения в общем чате
    bool m_messageGroupChat{};    // показывать сообщения группы в общем чате
    bool m_streamChatOnOff{};   // При перекдючении чатов - переключать канал сообщений
    int passwdEchoMode{};
    int m_numSpacingListGame{};     // растояние между элементами в list
    int m_statusClient{};             // статус клиента
    int m_debugToInfoLevel{1};      // вывод отладочной информации
    bool m_systemChatFlag{};
    bool m_logFileFlag{};               // флаг для логера
    bool m_tempLogFileFlag{};           // временный флаг
    bool m_flagDataToConsole{};         // отправка данных в консоль
    bool m_tempGlagDataToConsol{};      // временный флаг для консоли
    QString m_pathAndNameLogFile;       // путь и имя логера
    QString m_tempPathAndNameLogFile;   // промежуточное сохранение пути
    // ----     выводить дату и время
    bool m_dateTimeInfo{};
    bool m_dateYearInfo{};
    bool m_dateMonthInfo{};
    bool m_dateDayInfo{};
    bool m_dateHourInfo{};
    bool m_dateMinuteInfo{};
    bool m_dateSecondInfo{};
    bool m_temp_dateTimeInfo{};
    bool m_temp_dateYearInfo{};
    bool m_temp_dateMonthInfo{};
    bool m_temp_dateDayInfo{};
    bool m_temp_dateHourInfo{};
    bool m_temp_dateMinuteInfo{};
    bool m_temp_dateSecondInfo{};
    QString m_strDateYearInfo;
    QString m_strDateMonthInfo;
    QString m_strDateDayInfo;
    QString m_strDateHourInfo;
    QString m_strDateMinuteInfo;
    QString m_strDateSecondInfo;
    // ------       цвета
    // цвета для списка
    QColor m_colorGoPlaye;
    QColor m_colorChat;
    QColor m_colorBusy;
    QColor m_temp_colorGoPlaye;
    QColor m_temp_colorChat;
    QColor m_temp_colorBusy;
    QPalette m_paletteGoPlaye;
    QPalette m_paletteChat;
    QPalette m_paletteBusy;
    // цвета для чата
    QColor m_colorFonFormInfo;
    QColor m_temp_colorFonFormInfo;
    QPalette m_paletteFonFormInfo;
    //
    QColor m_colorFonFormPrivat;
    QColor m_temp_colorFonFormPrivat;
    QPalette m_paletteFonFormPrivat;
    //
    QColor m_colorFonFormGroup;
    QColor m_temp_colorFonFormGroup;
    QPalette m_paletteFonFormGroup;
    // текст
    // system
    QColor m_colorSystemInfo;
    QColor m_temp_colorSystemInfo;
    QPalette m_paletteSystemInfo;
    // debug
    QColor m_colordebug;
    QColor m_temp_colordebug;
    QPalette m_palettedebug;
    // chat push
    QColor m_colorChatPush;
    QColor m_temp_colorChatPush;
    QPalette m_paletteChatPush;
    // chat pull
    QColor m_colorChatPull;
    QColor m_temp_colorChatPull;
    QPalette m_paletteChatPull;
    // chat private
    QColor m_colorChatPrivate;
    QColor m_temp_colorChatPrivate;
    QPalette m_paletteChatPrivate;
    // chat Group --
    QColor m_colorChatGroup;
    QColor m_temp_colorChatGroup;
    QPalette m_paletteChatGroup;
    // -----        шpифт
    QFont m_systemFont;
    // system --
    int m_systemInfoIndexFond{};
    QFont m_systemInfoFont;
    bool m_systemInfoFontSelected{};    // шрифт выбран
    bool m_weightFontSystemInfo{};   // флаг для жирности шрифта
    bool m_italicFontSystemInfo{};      // для курсива
    bool m_underlineFontSystemInfo{};      // для подчеркнутого
    // debug --
    int m_debugIndexFond{};
    QFont m_debugFont;
    bool m_debugFontSelected{};     // шрифт выбран
    bool m_weightFontDebug{};       // флаг для жирности шрифта
    bool m_italicFontDebug{};       // для курсива
    bool m_underlineFontDebug{};    // для подчеркнутого
    // chat push --
    int m_chatPushIndexFond{};
    QFont m_chatPushFont;
    bool m_chatPushFontSelected{};        // шрифт выбран
    bool m_weightFontchatPush{};          // флаг для жирности шрифта
    bool m_italicFontchatPush{};          // для курсива
    bool m_underlineFontchatPush{};       // для подчеркнутого
    // chat pull --
    int m_chatPullIndexFond{};
    QFont m_chatPullFont;
    bool m_chatPullFontSelected{};        // шрифт выбран
    bool m_weightFontchatPull{};          // флаг для жирности шрифта
    bool m_italicFontchatPull{};          // для курсива
    bool m_underlineFontchatPull{};       // для подчеркнутого
    // chat Private --
    int m_chatPrivateIndexFond{};
    QFont m_chatPrivateFont;
    bool m_chatPrivateFontSelected{};        // шрифт выбран
    bool m_weightFontchatPrivate{};          // флаг для жирности шрифта
    bool m_italicFontchatPrivate{};          // для курсива
    bool m_underlineFontchatPrivate{};       // для подчеркнутого
    // chat Group --
    int m_chatGroupIndexFond{};
    QFont m_chatGroupFont;
    bool m_chatGroupFontSelected{};        // шрифт выбран
    bool m_weightFontchatGroup{};          // флаг для жирности шрифта
    bool m_italicFontchatGroup{};          // для курсива
    bool m_underlineFontchatGroup{};       // для подчеркнутого
    // кнопки для чата
    bool m_clearChat{};     // вывод кнопки очистить чат
    bool m_copyChat{};      // вывод кнопки копировать
    bool m_saveFileChat{};  // вывод кнопки сохранить чат в файле
    // для дублирование сообщений в другии чаты
    bool m_copyDebugForChat{};
    bool m_copySystemForChat{};

    // ------
    int m_countToPlayerServer{};    // количество играков на сервере
    QString pathSaveChatFile;       // путь для сохранения чата в файле
    QStringList m_listClient;      // здесь храниться выбранный клиент для отправки личного сообщения
    bool m_privateclick{};      // нажата ли кнопка для отправки личных сообщений (переключение)
    int m_countMessagePrivateChat = 0;  // сколько всего пришло личных сообщений

    // ------------------------------------
    // блокировка для отключения спама
    int m_blokConnectServer{};
    int m_stepBlokTimer = 10;
    QTimer m_blokTimer;
    // ----------------------------

    // -----------------------------------
    // формы приглашения
    QVector<int> vecOpponentKNB;   // список всех участников группы KNB

    int idOpponent_1 = -1, idOpponent_2 = -1, idOpponent_3 = -1;      // опоненты
    QString gameOffer_1;
    QString gameOffer_2;
    QString gameOffer_3;

    void closeGameOffer(QString str);
    void offerToJoin(int idOpp, QString game, bool grop = false);
    // -----------------------------------

    // ------------------------------------
    // оффлай игра для KNB
    QTimer m_offlineGameKNBTimer;
    int m_stepOffline{};
    int m_maxStepPointOffline{};    // интервал работы таймера и прогрес бара
    int m_answerRandomKNB{};        // рандомное значение для ответа
    int m_codeAnswerPlayerKNB{};    // код ответа, что именно выбрал игорк

    void resultOfflineAnswerKNB();
    // ------------------------------------


    // ------------------------------------
    // группы
    int m_countGroup = 0;
    int m_idHost_1{};   // id хоста
    int m_idHost_2{};
    int m_idHost_3{};
    int m_idHost_KNB{};

    QVector<int> vecGropu_1;
    QVector<int> vecGropu_2;
    QVector<int> vecGropu_3;

    bool m_group_1 = false;     // состоит ли клиент в группе
    bool m_group_2 = false;
    bool m_group_3 = false;
    bool m_groupKNB = false;

    bool m_hostGroup_1 = false;     // я хост
    bool m_hostGroup_2 = false;
    bool m_hostGroup_3 = false;
    bool m_hostGroup_KNB = false;

    // основной метод для очистки данных при удаление клинта и группы
    void chekDelClientGroup(QString group);
    void chekClientListGroup();
    void chekClientGroup();
    void chekMyClientGroup();
    // для вызова внутри chekClientGroup
    bool chekClientStatus(QVector<int>& vecGroup, bool hostGroup);
    //void textClientLabelGroup(int numGroup);    // запись информации в label group
    void textClientLabelGroup(QLabel* lb_group, QVector<int> vec, int idHost, bool host);
    void textNameChatGroup();       // может ли клиент писатьв группу
    void clearDataClientGroup();    // очистка данных если клиент покинул все круппы
    void setTextLabelGroup(QString group);  // в каких группах состоит клиент
    void delDataGroup(QString group);   // удалить данные о группе

    void delClientGroupKNB();

    void leaveClientTotalGroup();
    // ------------------------------------






    PackegeDate packege_date;  // удалить
    QString answer = "";    // возможно тоже удалить




    void blocFormButtonKNB(bool flag = true);     // блокировка при ходе
    void infoMotionFormKNB(QString str);       // вывод информации пари ходе
    void infoMotionFormKNB(QString str, QVector<int> vec);  // вывод информации пари ходе


    // -----------------------------------------
    // таймер для отчёта времени поиска группы и ожидания хода
    QTimer timerFindGroupsAndMotionKNB; //timer
    int m_step{};
    int m_stepOpposite = 60;
    bool m_findStartKNB{};      // флаг что поиск запущени
    bool m_clientRoot = true;

    bool m_timerFormKNBStrart = false;  // флаг обозначающий старт таймера

    bool m_motionOppjnentKNB = false;  // ход противника
    bool m_motionKNB = false;       // ход игрока

    void stopTimerFindGroups(QString str);
    void stopTimerFindGroups();     // остановка таймера при покидание группы во время игры
    void endTimeFindGroupsKNB();
    void stopTimerFindGroupsAndMotionKNB();
    // -----------------------------------------

    void sendToServer(Tranciver_commmand command, int idOpponent = -1, QString gameOffer = "");

    void readingConfig(QString path = "./client.conf");
    void writeConfig(QString path = "./client.conf");

    void enabledFieldForm(bool flag = true);    // блокировка полей регестрации при подключении
    void enabledGameForm(bool flag = true);     // блокировка вкладки игры

    void visibleGameKNB(bool flag = true);      // сокрытие игровых кнопок в KNB
    void enabledGameKNB(bool flag = true);      // блокировка клавиш при принятия приглашения

    void chatBlok(bool flag = true);    // блокировка чата

    void connectToServer(bool reg = false);     // подключение

    void debugToInfo(QString str, int level = 4);   // вывод дебага
    void systemChatInfo(QString str);   // вывод системной информации
    void strDateTimeInfo();      // установка вывода даты в дебаг
    void enabledDataTime();     // блокировка вывода формата даты

    void questionFormKNB(QString strText,
                         QString strButtonYes,
                         QString strButtomNo);  // вопрос на форме KNB
    bool questionFormKNBVisible{};  // переменная открыто ли форма вопроса
    void questionFormKNBDefolt();

    // -----------------
    QStringList m_playerList;   // список клиентов
    void redrawListClient();  // перерисовать список клиентов
    // -----------------

    // -------------------
    // чат
    void chatPullClient(int id, QString str);      // принятие сообщение клиентам
    void chatPullPrivate(int id, QString str);     // принятие личных сообщений
    void statusChatPrivate(bool flag = true);      // статус можно ли отправлять личные сообщения
    void switchChatInfo();      // сообщения при переключении чата на общий
    // -------------------


        // удалить
    void sendDataToServer(Tranciver_commmand command, const QVariantList& data);

public:
    explicit MyClient(QWidget *parent = 0);
    ~MyClient();

    bool getFlagLogFile();
    QString getPathLogFile();

private:
    Ui::MyClient *ui;

private slots:

    void slotReabyRead();
    void slotError();

    void slotConnected();

    // -----------------------------------------
    // таймеры
    void slotProgressFindGroups();
    void slotBlokConnectServer();
    void slotofflineGameKNB();
    // -----------------------------------------

    void on_pb_stone_clicked();
    void on_pb_scissors_clicked();
    void on_pb_paper_clicked();
    void on_pb_close_clicked();
    void on_pb_anonymous_clicked();
    void on_cb_loginSave_clicked();
    void on_pb_log_in_clicked();
    void on_cb_isSettings_clicked();
    void on_cb_passwdSave_clicked();
    void on_cb_systemChat_clicked();
    void on_cb_totalChat_clicked();
    void on_cb_privatChat_clicked();
    void on_pb_chatPush_clicked();
    void on_cb_offline_clicked();
    void on_pb_registServer_clicked();
    void on_rb_pass_clicked();
    void on_rb_passEcho_clicked();
    void on_rb_passNormal_clicked();
    void on_rb_passNoEcho_clicked();
    void on_sb_sizeItemListGame_valueChanged(int arg1);
    void on_cb_statusClient_activated(int index);
    void on_pb_gameStartKNB_clicked();
    void on_hs_countPlayerKNB_valueChanged(int value);
    void on_sb_countPlayerKNB_valueChanged(int arg1);
    void on_pb_leaveGameKNB_clicked();
    void on_gameListWidget_currentTextChanged(const QString &currentText);
    void on_hs_levelInfo_valueChanged(int value);
    void on_pb_questionKNBNo_clicked();
    void on_pb_questionKNBYes_clicked();
    void on_pb_formOneRejoction_clicked();
    void on_pb_acceptOne_clicked();
    void on_pb_acceptTwo_clicked();
    void on_pb_formTwoRejoction_clicked();
    void on_pb_acceptThree_clicked();
    void on_pb_formThreeRejoction_clicked();
    void on_pb_ping_clicked();
    void on_pb_localhost_clicked();
    void on_le_ipAddress_textEdited(const QString &arg1);
    void on_pb_saveIpAdressAndPort_clicked();
    void on_pb_cancel_ipAdressAndPort_clicked();
    void on_le_port_textEdited(const QString &arg1);
    void on_cb_logFile_clicked();
    void on_pb_saveLogFile_clicked();
    void on_pb_cancelLogFile_clicked();
    void on_le_pathLogFile_textEdited(const QString &arg1);
    void on_rb_logConsol_clicked();
    void on_rb_textLogFile_clicked();
    void on_pb_dirPathLog_clicked();
    void on_rb_notDateTime_clicked();
    void on_rb_dateTimeOut_clicked();
    void on_ch_dataTime_clic();
    void on_pb_saveInfoDateDebug_clicked();
    void on_pb_canselDataInfoDebug_clicked();
    void on_pb_colorOnline_clicked();
    void on_pb_colorChat_clicked();
    void on_pb_colorBusy_clicked();
    void on_pb_colorSystemInfo_clicked();
    void on_pb_colorDebug_clicked();
    void on_cb_debugFondSize_activated(int index);
    void on_pb_debugFond_clicked();
    void on_rb_totalChat_clicked();
    void on_rb_privatChat_clicked();
    void on_rb_groupChat_clicked();
    void on_pb_colorFonIndo_clicked();
    void on_cb_systemInfoFondSize_activated(int index);
    void on_pb_systemInfoFond_clicked();
    void on_cb_weightFontDebug_clicked();
    void on_cb_italicFontDebug_clicked();
    void on_cb_underlineFontDebug_clicked();
    void on_cb_weightFontSystemInfo_clicked();
    void on_cb_italicFontSystemInfo_clicked();
    void on_cb_underlineFontSystemInfo_clicked();
    void on_pb_colorChatPush_clicked();
    void on_cb_chatPushFontSize_activated(int index);
    void on_pb_chatPushFont_clicked();
    void on_cb_weightFontChatPush_clicked();
    void on_cb_italicFontChatPush_clicked();
    void on_cb_underlineFontChatPush_clicked();
    void on_pb_colorChatPull_clicked();
    void on_cb_chatPullFontSize_activated(int index);
    void on_pb_chatPullFont_clicked();
    void on_cb_weightFontChatPull_clicked();
    void on_cb_italicFontChatPull_clicked();
    void on_cb_underlineFontChatPull_clicked();
    void on_cb_clearText_clicked();
    void on_pb_clearChat_clicked();
    void on_pb_cutChat_clicked();
    void on_cb_copuText_clicked();
    void on_pb_saveFileChat_clicked();
    void on_cb_saveFileChat_clicked();
    void on_cb_debugCopyForChat_clicked();
    void on_cb_systemCopyForChat_clicked();
    void on_pb_colorFonPrivat_clicked();
    void on_pb_colorFonGroup_clicked();
    void on_cb_botPushKNB_clicked();
    void on_pb_chatPrivatemaesage_clicked();
    void on_cb_messagePrivatTotalChat_clicked();
    void on_pb_colorChatPrivate_clicked();
    void on_cb_chatPrivateFontSize_activated(int index);
    void on_pb_chatPrivateFont_clicked();
    void on_cb_weightFontChatPrivate_clicked();
    void on_cb_italicFontChatPrivate_clicked();
    void on_cb_underlineFontChatPrivate_clicked();
    void on_pb_break_clicked();
    void on_pb_infoPrivatPull_clicked();
    void on_ch_streamChat_clicked();
    void on_pb_offerClientGroup_clicked();
    void on_b_nameOfferClientGroup_activated(const QString &arg1);
    void on_cb_nameChatGroup_activated(const QString &arg1);
    void on_pb_colorChatGroup_clicked();
    void on_cb_chatGroupFontSize_activated(int index);
    void on_pb_chatGroupFont_clicked();
    void on_cb_weightFontChatGroup_clicked();
    void on_cb_italicFontChatGroup_clicked();
    void on_cb_underlineFontChatGroup_clicked();
    void on_cb_messageGroupTotalChat_clicked();
};

#endif // MYCLIENT_H
