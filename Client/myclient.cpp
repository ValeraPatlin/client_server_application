#include "myclient.h"
#include "ui_myclient.h"

MyClient::MyClient(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MyClient)
{
    ui->setupUi(this);

    m_pTcpSocket = new QTcpSocket(this);

    connect(m_pTcpSocket, SIGNAL(connected()), SLOT(slotConnected()));
    connect(m_pTcpSocket, SIGNAL(readyRead()), SLOT(slotReabyRead()));
    connect(m_pTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(slotError(QAbstractSocket::SocketError)));

    readingConfig();    // считывание из конфига

    // размер общей формы настроек (там где доп настройки и ip)
    ui->gb_settings->setMaximumHeight(180);


    // ---------------------------------
    // определяем системный шрифт
    //m_systemFont;

    // ----------------------------------
    ui->le_login->setText(strLogun);
    ui->le_passwd->setText(strPasswd);
    ui->le_login->setClearButtonEnabled(true);
    ui->le_passwd->setClearButtonEnabled(true);


    // ----------------------------------
    // подключение
    ui->le_ipAddress->setText(strIPAddres);
    ui->le_port->setText(QString::number(m_port));
    ui->le_port->setValidator(new QIntValidator(0, 65535, this));
    ui->le_ipAddress->setClearButtonEnabled(true);
    ui->le_port->setClearButtonEnabled(true);
    ui->le_ipAddress->setPlaceholderText("Введите ip адрес");
    ui->le_port->setPlaceholderText("Введите порт");

    m_tempStrIPAddress = strIPAddres;       // временное сохранение ip адреса и порта
    m_tempPort = m_port;

    ui->lb_infoReg->setVisible(false);
    ui->lb_infoPing->setText("");
    ui->lb_infoPing->setMinimumWidth(100);
    ui->pb_localhost->setToolTip("Установить локальный ip адрес");
    ui->pb_ping->setToolTip("Послать команду пинг к подключаемой машине для "
                            "проверки её доступности.\n"
                            "ВНИМАНИЕ!!! Команда пинг не учитывает порт и наличие установленого "
                            "на машине програмного обеспечения!");

    ui->pb_saveIpAdressAndPort->setVisible(false);
    ui->pb_cancel_ipAdressAndPort->setVisible(false);
    // ----------------------------------


    // ------------------------------------
    // чат
    ui->cb_systemChat->setChecked(m_systemChatFlag);
    ui->cb_totalChat->setChecked(m_totalChat);
    ui->cb_privatChat->setChecked(m_privatChat);
    ui->ch_streamChat->setChecked(m_streamChatOnOff);
    ui->cb_debugCopyForChat->setChecked(m_copyDebugForChat);
    ui->cb_systemCopyForChat->setChecked(m_copySystemForChat);
    ui->lb_messageText->setWordWrap(true);
    ui->lb_textInfoGroup->setText("Вы не состоите в группе.");
    ui->cb_nameChatGroup->setVisible(false);
    ui->pb_offerClientGroup->setEnabled(false);
    ui->b_nameOfferClientGroup->setEnabled(false);
    statusChatPrivate(false);



    // информация о сообщениях если отк. дублирование
    ui->gb_infoPullMessage->setVisible(false);
    ui->gb_infoPullMessage->setMaximumHeight(80);
    ui->pb_infoPrivatPull->setIcon(QIcon(":/icon/icon/kn.png"));

    // личные сообщения ----
    ui->pb_chatPrivatemaesage->setToolTip("Переключиться на приватный чат.");
    m_privateclick = false;
    // Показывать личные сообщения в общем чате
    ui->cb_messagePrivatTotalChat->setChecked(m_messagePrivateChat);
    // --------

    // Показывать сообщения группы в общем чате
    ui->cb_messageGroupTotalChat->setChecked(m_messageGroupChat);

    ui->te_info->setReadOnly(true);
    ui->te_info->setTextInteractionFlags(Qt::NoTextInteraction | Qt::TextSelectableByMouse);
    ui->te_info->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->te_info->setMinimumSize(250, 260);

    ui->te_groupChat->setReadOnly(true);
    ui->te_groupChat->setTextInteractionFlags(Qt::NoTextInteraction);
    ui->te_groupChat->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->te_groupChat->setMinimumSize(250, 260);

    ui->te_privatChat->setReadOnly(true);
    ui->te_privatChat->setTextInteractionFlags(Qt::NoTextInteraction);
    ui->te_privatChat->setWordWrapMode(QTextOption::WrapAtWordBoundaryOrAnywhere);
    ui->te_privatChat->setMinimumSize(250, 260);

    ui->le_message->setClearButtonEnabled(true);
    ui->le_message->setPlaceholderText("Введите сообщение");

    // кнопки для чата
    ui->cb_clearText->setChecked(m_clearChat);
    ui->pb_clearChat->setVisible(m_clearChat);
    ui->cb_copuText->setChecked(m_copyChat);
    ui->pb_cutChat->setVisible(m_copyChat);
    ui->cb_saveFileChat->setChecked(m_saveFileChat);
    ui->pb_saveFileChat->setVisible(m_saveFileChat);

    pathSaveChatFile = QDir::currentPath(); // путь для сохранения чата в файле

    // для общего и группового чата
    ui->lb_messageText->setText("Ваше сообщение отправиться в общий чат");

    if (m_infoWinChat)  // общий чат
    {
        ui->rb_totalChat->setChecked(true);
        ui->te_privatChat->setVisible(false);
        ui->te_groupChat->setVisible(false);
    }
    else if (m_privatWinChat)   // личные сообщения
    {
        ui->rb_privatChat->setChecked(true);
        ui->te_info->setVisible(false);
        ui->te_groupChat->setVisible(false);
        on_pb_chatPrivatemaesage_clicked();
    }
    else if (m_groupWinChat)    // групповой чат
    {
        ui->rb_groupChat->setChecked(true);
        ui->te_info->setVisible(false);
        ui->te_privatChat->setVisible(false);
    }

    // цвет

    // цвет фона общий чат
    m_paletteFonFormInfo = ui->pb_colorFonIndo->palette();
    m_paletteFonFormInfo.setColor(QPalette::Button, m_colorFonFormInfo);
    ui->pb_colorFonIndo->setPalette(m_paletteFonFormInfo);
    ui->pb_colorFonIndo->setAutoFillBackground(true);
    m_temp_colorFonFormInfo = m_colorFonFormInfo;
    ui->te_info->setStyleSheet(QString("background-color: %1").arg(m_colorFonFormInfo.name()));

    // цвет фона личные
    m_paletteFonFormPrivat = ui->pb_colorFonPrivat->palette();
    m_paletteFonFormPrivat.setColor(QPalette::Button, m_colorFonFormPrivat);
    ui->pb_colorFonPrivat->setPalette(m_paletteFonFormPrivat);
    ui->pb_colorFonPrivat->setAutoFillBackground(true);
    m_temp_colorFonFormPrivat = m_colorFonFormPrivat;
    ui->te_privatChat->setStyleSheet(QString("background-color: %1").
                                     arg(m_colorFonFormPrivat.name()));

    // цвет фона группа
    m_paletteSystemInfo = ui->pb_colorFonGroup->palette();
    m_paletteSystemInfo.setColor(QPalette::Button, m_colorFonFormGroup);
    ui->pb_colorFonGroup->setPalette(m_paletteSystemInfo);
    ui->pb_colorFonGroup->setAutoFillBackground(true);
    m_temp_colorSystemInfo = m_colorFonFormGroup;
    ui->te_groupChat->setStyleSheet(QString("background-color: %1").arg(m_colorFonFormGroup.name()));
    // ---


    // системные сообщения -------
    m_paletteSystemInfo = ui->pb_colorSystemInfo->palette();
    m_paletteSystemInfo.setColor(QPalette::Button, m_colorSystemInfo);
    ui->pb_colorSystemInfo->setPalette(m_paletteSystemInfo);
    ui->pb_colorSystemInfo->setAutoFillBackground(true);
    m_temp_colorSystemInfo = m_colorSystemInfo;
    // размер шрифта
    ui->cb_systemInfoFondSize->setCurrentIndex(m_systemInfoIndexFond);
    m_systemInfoFont.setPointSize(ui->cb_systemInfoFondSize->currentText().toInt());

    // установка шрифта
    if (m_systemInfoFont.family() == "")
    {
        m_systemInfoFont.setFamily(m_systemFont.family());
    }

    ui->pb_systemInfoFond->setText(m_systemInfoFont.family());

    // жир, курсив, черта
    ui->cb_weightFontSystemInfo->setChecked(m_weightFontSystemInfo);
    m_systemInfoFont.setBold(m_weightFontSystemInfo);
    ui->cb_italicFontSystemInfo->setChecked(m_italicFontSystemInfo);
    m_systemInfoFont.setItalic(m_italicFontSystemInfo);
    ui->cb_underlineFontSystemInfo->setChecked(m_underlineFontSystemInfo);
    m_systemInfoFont.setUnderline(m_underlineFontSystemInfo);
    // -----------


    // debug -------
    m_palettedebug = ui->pb_colorDebug->palette();
    m_palettedebug.setColor(QPalette::Button, m_colordebug);
    ui->pb_colorDebug->setPalette(m_palettedebug);
    ui->pb_colorDebug->setAutoFillBackground(true);
    m_temp_colordebug = m_colordebug;
    // размер шрифта
    ui->cb_debugFondSize->setCurrentIndex(m_debugIndexFond);
    m_debugFont.setPointSize(ui->cb_debugFondSize->currentText().toInt());

    // установка шрифта
    if (m_debugFont.family() == "")
    {
        m_debugFont.setFamily(m_systemFont.family());
    }

    ui->pb_debugFond->setText(m_debugFont.family());

    // жир, курсив, черта
    ui->cb_weightFontDebug->setChecked(m_weightFontDebug);
    m_debugFont.setBold(m_weightFontDebug);
    ui->cb_italicFontDebug->setChecked(m_italicFontDebug);
    m_debugFont.setItalic(m_italicFontDebug);
    ui->cb_underlineFontDebug->setChecked(m_underlineFontDebug);
    m_debugFont.setUnderline(m_underlineFontDebug);
    // ---------


    // oтправленные сообщения -------
    m_paletteChatPush = ui->pb_colorChatPush->palette();
    m_paletteChatPush.setColor(QPalette::Button, m_colorChatPush);
    ui->pb_colorChatPush->setPalette(m_paletteChatPush);
    ui->pb_colorChatPush->setAutoFillBackground(true);
    m_temp_colorChatPush = m_colorChatPush;
    // размер шрифта
    ui->cb_chatPushFontSize->setCurrentIndex(m_chatPushIndexFond);
    m_chatPushFont.setPointSize(ui->cb_chatPushFontSize->currentText().toInt());

    // установка шрифта
    if (m_chatPushFont.family() == "")
    {
        m_chatPushFont.setFamily(m_systemFont.family());
    }

    ui->pb_chatPushFont->setText(m_chatPushFont.family());

    // жир, курсив, черта
    ui->cb_weightFontChatPush->setChecked(m_weightFontchatPush);
    m_chatPushFont.setBold(m_weightFontchatPush);
    ui->cb_italicFontChatPush->setChecked(m_italicFontchatPush);
    m_chatPushFont.setItalic(m_italicFontchatPush);
    ui->cb_underlineFontChatPush->setChecked(m_underlineFontchatPush);
    m_chatPushFont.setUnderline(m_underlineFontchatPush);


    // принятые сообщения   ----------
    m_paletteChatPull = ui->pb_colorChatPull->palette();
    m_paletteChatPull.setColor(QPalette::Button, m_colorChatPull);
    ui->pb_colorChatPull->setPalette(m_paletteChatPull);
    ui->pb_colorChatPull->setAutoFillBackground(true);
    m_temp_colorChatPull = m_colorChatPull;
    // размер шрифта
    ui->cb_chatPullFontSize->setCurrentIndex(m_chatPullIndexFond);
    m_chatPullFont.setPointSize(ui->cb_chatPullFontSize->currentText().toInt());

    // установка шрифта
    if (m_chatPullFont.family() == "")
    {
        m_chatPullFont.setFamily(m_systemFont.family());
    }

    ui->pb_chatPullFont->setText(m_chatPullFont.family());

    // жир, курсив, черта
    ui->cb_weightFontChatPull->setChecked(m_weightFontchatPull);
    m_chatPullFont.setBold(m_weightFontchatPull);
    ui->cb_italicFontChatPull->setChecked(m_italicFontchatPull);
    m_chatPullFont.setItalic(m_italicFontchatPull);
    ui->cb_underlineFontChatPull->setChecked(m_underlineFontchatPull);
    m_chatPullFont.setUnderline(m_underlineFontchatPull);

    // личные сообщения     Private ----------------
    m_paletteChatPrivate = ui->pb_colorChatPrivate->palette();
    m_paletteChatPrivate.setColor(QPalette::Button, m_colorChatPrivate);
    ui->pb_colorChatPrivate->setPalette(m_paletteChatPrivate);
    ui->pb_colorChatPrivate->setAutoFillBackground(true);
    m_temp_colorChatPrivate = m_colorChatPrivate;
    // размер шрифта
    ui->cb_chatPrivateFontSize->setCurrentIndex(m_chatPushIndexFond);
    m_chatPrivateFont.setPointSize(ui->cb_chatPrivateFontSize->currentText().toInt());

    // установка шрифта
    if (m_chatPrivateFont.family() == "")
    {
        m_chatPrivateFont.setFamily(m_systemFont.family());
    }

    ui->pb_chatPrivateFont->setText(m_chatPrivateFont.family());

    // жир, курсив, черта
    ui->cb_weightFontChatPrivate->setChecked(m_weightFontchatPrivate);
    m_chatPrivateFont.setBold(m_weightFontchatPrivate);
    ui->cb_italicFontChatPrivate->setChecked(m_italicFontchatPrivate);
    m_chatPrivateFont.setItalic(m_italicFontchatPrivate);
    ui->cb_underlineFontChatPrivate->setChecked(m_underlineFontchatPrivate);
    m_chatPrivateFont.setUnderline(m_underlineFontchatPrivate);

    // чат группы     Group ----------------
    m_paletteChatGroup = ui->pb_colorChatGroup->palette();
    m_paletteChatGroup.setColor(QPalette::Button, m_colorChatGroup);
    ui->pb_colorChatGroup->setPalette(m_paletteChatGroup);
    ui->pb_colorChatGroup->setAutoFillBackground(true);
    m_temp_colorChatGroup = m_colorChatGroup;
    // размер шрифта
    ui->cb_chatGroupFontSize->setCurrentIndex(m_chatGroupIndexFond);
    m_chatGroupFont.setPointSize(ui->cb_chatGroupFontSize->currentText().toInt());

    // установка шрифта
    if (m_chatGroupFont.family() == "")
    {
        m_chatGroupFont.setFamily(m_systemFont.family());
    }

    ui->pb_chatGroupFont->setText(m_chatGroupFont.family());

    // жир, курсив, черта
    ui->cb_weightFontChatGroup->setChecked(m_weightFontchatGroup);
    m_chatGroupFont.setBold(m_weightFontchatGroup);
    ui->cb_italicFontChatGroup->setChecked(m_italicFontchatGroup);
    m_chatGroupFont.setItalic(m_italicFontchatGroup);
    ui->cb_underlineFontChatGroup->setChecked(m_underlineFontchatGroup);
    m_chatGroupFont.setUnderline(m_underlineFontchatGroup);
    // ------------------------------------



    // ------------------------------------
    // отображение списка клиентов
    ui->gameListWidget->setSpacing(m_numSpacingListGame);
    ui->gameListWidget->setMinimumWidth(560);
    ui->sb_sizeItemListGame->setWrapping(true);
    ui->sb_sizeItemListGame->setValue(m_numSpacingListGame);
    ui->sb_sizeItemListGame->setRange(0, 10);
    ui->tab_listClientAndGroup->setTabEnabled(1, false);    // вкладка группы
    // статус клиента
    ui->cb_statusClient->setCurrentIndex(m_statusClient);
    // цвет рамки
    m_paletteGoPlaye = ui->pb_colorOnline->palette();
    m_paletteGoPlaye.setColor(QPalette::Button, m_colorGoPlaye);
    ui->pb_colorOnline->setPalette(m_paletteGoPlaye);
    ui->pb_colorOnline->setAutoFillBackground(true);

    m_paletteChat = ui->pb_colorChat->palette();
    m_paletteChat.setColor(QPalette::Button, m_colorChat);
    ui->pb_colorChat->setPalette(m_paletteChat);
    ui->pb_colorChat->setAutoFillBackground(true);

    m_paletteBusy = ui->pb_colorBusy->palette();
    m_paletteBusy.setColor(QPalette::Button, m_colorBusy);
    ui->pb_colorBusy->setPalette(m_paletteBusy);
    ui->pb_colorBusy->setAutoFillBackground(true);

    m_temp_colorGoPlaye = m_colorGoPlaye;
    m_temp_colorChat = m_colorChat;
    m_temp_colorBusy = m_colorBusy;
    // -------------------------------------



    // ------------------------------------- !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    // установка таймера
    timerFindGroupsAndMotionKNB.setInterval(1000);
    connect(&timerFindGroupsAndMotionKNB, SIGNAL(timeout()), SLOT(slotProgressFindGroups()));

    m_blokTimer.setInterval(1000);
    connect(&m_blokTimer, SIGNAL(timeout()), SLOT(slotBlokConnectServer()));

    m_maxStepPointOffline = 4;  // интервал работы таймера и прогрес бара
    m_offlineGameKNBTimer.setInterval(1000);
    connect(&m_offlineGameKNBTimer, SIGNAL(timeout()), SLOT(slotofflineGameKNB()));

    ui->pb_timerStatuStepKNB->setRange(0, m_maxStepPointOffline);
    ui->pb_timerStatuStepKNB->setValue(0);
    ui->pb_timerStatuStepKNB->setTextVisible(false);
    ui->gb_offlineStatus->setVisible(false);    // поле будет видно только в оФФлайне
    ui->lb_statusInfoStepKNB->clear();     // поле для информации о ходе компа в офaлайне


    ui->pb_timerFindGroups->setRange(0, 60);
    ui->pb_timerFindGroups->setValue(0);
    ui->pb_timerFindGroups->setTextVisible(false);
    ui->pb_timerFindGroups->setMinimumHeight(35);
    m_findStartKNB = false;

    ui->lb_statusFindGrup->setText("");
    ui->lb_stepOpp->setNum(m_stepOpposite);
    // -------------------------------------




    // -------------------------------------
    // формат вывода пароля
    if (passwdEchoMode == 1)    // показывать ***
    {
        ui->le_passwd->setEchoMode(QLineEdit::Password);
        ui->rb_pass->setChecked(true);
    }
    else if (passwdEchoMode == 2)   // при потере фокуса
    {
        ui->le_passwd->setEchoMode(QLineEdit::PasswordEchoOnEdit);
        ui->rb_passEcho->setChecked(true);
    }
    else if (passwdEchoMode == 3)   // показывать полностью
    {
        ui->le_passwd->setEchoMode(QLineEdit::Normal);
        ui->rb_passNormal->setChecked(true);
    }
    else if (passwdEchoMode == 4)   // скрыть полностью
    {
        ui->le_passwd->setEchoMode(QLineEdit::NoEcho);
        ui->rb_passNoEcho->setChecked(true);
    }
    // ------------------------------------


    // --------------------------------------
    // Настройка вывода информации
    ui->hs_levelInfo->setRange(0, 4);
    ui->hs_levelInfo->setTickPosition(QSlider::TicksAbove);
    ui->hs_levelInfo->setTickInterval(1);
    ui->hs_levelInfo->setValue(m_debugToInfoLevel);

    if (m_dateTimeInfo)
    {
        ui->rb_dateTimeOut->setChecked(true);
        ui->ch_yearInfo->setEnabled(true);
        ui->ch_monthInfo->setEnabled(true);
        ui->ch_dayInfo->setEnabled(true);
        ui->ch_hourInfo->setEnabled(true);
        ui->ch_minuteInfo->setEnabled(true);
        ui->ch_secondInfo->setEnabled(true);
    }
    else
    {
        ui->rb_notDateTime->setChecked(true);
        ui->ch_yearInfo->setEnabled(false);
        ui->ch_monthInfo->setEnabled(false);
        ui->ch_dayInfo->setEnabled(false);
        ui->ch_hourInfo->setEnabled(false);
        ui->ch_minuteInfo->setEnabled(false);
        ui->ch_secondInfo->setEnabled(false);
    }

    ui->ch_yearInfo->setChecked(m_dateYearInfo);
    ui->ch_monthInfo->setChecked(m_dateMonthInfo);
    ui->ch_dayInfo->setChecked(m_dateDayInfo);
    ui->ch_hourInfo->setChecked(m_dateHourInfo);
    ui->ch_minuteInfo->setChecked(m_dateMinuteInfo);
    ui->ch_secondInfo->setChecked(m_dateSecondInfo);

    m_temp_dateTimeInfo = m_dateTimeInfo;
    m_temp_dateYearInfo = m_dateYearInfo;
    m_temp_dateMonthInfo = m_dateMonthInfo;
    m_temp_dateDayInfo = m_dateDayInfo;
    m_temp_dateHourInfo = m_dateHourInfo;
    m_temp_dateMinuteInfo = m_dateMinuteInfo;
    m_temp_dateSecondInfo = m_dateSecondInfo;

    strDateTimeInfo();

    connect(ui->ch_yearInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));
    connect(ui->ch_monthInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));
    connect(ui->ch_dayInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));
    connect(ui->ch_hourInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));
    connect(ui->ch_minuteInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));
    connect(ui->ch_secondInfo, SIGNAL(clicked()), SLOT(on_ch_dataTime_clic()));

    ui->pb_saveInfoDateDebug->setVisible(false);
    ui->pb_canselDataInfoDebug->setVisible(false);
    // --------------------------------------


    // ------------------------------------------
    // логер
    ui->cb_logFile->setChecked(m_logFileFlag);
    ui->le_pathLogFile->setEnabled(m_logFileFlag);
    ui->le_pathLogFile->setText(m_pathAndNameLogFile);
    ui->pb_saveLogFile->setVisible(false);
    ui->pb_cancelLogFile->setVisible(false);
    ui->lb_logInfo->setVisible(false);
    m_tempGlagDataToConsol = m_flagDataToConsole;

    if (m_flagDataToConsole)
    {
        ui->rb_textLogFile->setChecked(true);
    }
    else
    {
        ui->rb_logConsol->setChecked(true);
    }
    // ------------------------------------------


    // -------------------------------------------------
    // блокировка и сокрытие кнопок и вкладок
    enabledGameForm(false); // блокировка вкладки игры

    ui->gb_settingsAd->setVisible(false);       // сокрытие доп настроек
    chatBlok(false);     // блокировка чата
    ui->cb_systemChat->setChecked(m_systemChatFlag);

    ui->gb_game_1->setVisible(false);
    ui->gb_game_1->setMaximumHeight(110);
    ui->gb_game_2->setVisible(false);
    ui->gb_game_2->setMaximumHeight(110);
    ui->gb_game_3->setVisible(false);
    ui->gb_game_3->setMaximumHeight(110);

    // сообшение о некоректных данных пришедших с сервера
    ui->gb_errorInfo->setVisible(false);
    ui->gb_errorInfo->setMaximumHeight(150);
    ui->lb_errorText->setWordWrap(true);



    // КНБ
    visibleGameKNB(false);
    ui->sb_countPlayerKNB->setButtonSymbols(QAbstractSpinBox::NoButtons);
    ui->hs_countPlayerKNB->setTickPosition(QSlider::TicksAbove);
    ui->hs_countPlayerKNB->setTickInterval(1);
    ui->hs_countPlayerKNB->setRange(2, 10);
    ui->gb_questionKNB->setVisible(false);      // закрытие вопроса
    ui->lb_infoGameKNB->setText("");        // поле информвции
    m_statusFormKNB = statusDefolt;
    questionFormKNBVisible = false;
    // ------------------------------------------------
}

MyClient::~MyClient()
{
    delete ui;
}



// -----------------------------------------------------------------
// методы считывание и записи в конфиг
void MyClient::readingConfig(QString path)
{
    QSettings m_settings(path, QSettings::IniFormat);
    m_settings.beginGroup("Settinhs");

    strIPAddres = m_settings.value("strIPAddres", "127.0.0.1").toString();
    strLogun = m_settings.value("strLogun", "").toString();
    m_port = m_settings.value("port", 2323).toInt();
    strPasswd = m_settings.value("strPasswd", "").toString();
    m_totalChat = m_settings.value("totalChat", true).toBool();
    m_privatChat = m_settings.value("privatChat", true).toBool();
    m_infoWinChat = m_settings.value("infoWinChat", true).toBool();
    m_privatWinChat = m_settings.value("privatWinChat", false).toBool();
    m_groupWinChat = m_settings.value("groupWinChat", false).toBool();
    m_messagePrivateChat = m_settings.value("messagePrivateChat", false).toBool();
    m_messageGroupChat = m_settings.value("messageGroupChat", false).toBool();
    passwdEchoMode = m_settings.value("passwdEchoMode", 1).toInt();
    m_numSpacingListGame = m_settings.value("spacingListGame", 0).toInt();
    m_statusClient = m_settings.value("statusClient", 0).toInt();
    m_debugToInfoLevel= m_settings.value("debugToInfoLevel", 1). toInt();
    m_systemChatFlag = m_settings.value("systemChatFlag", true).toBool();
    m_logFileFlag= m_settings.value("logFileFlag", false).toBool();
    m_pathAndNameLogFile = m_settings.value("pathAndNameLogFile", "").toString();
    m_flagDataToConsole = m_settings.value("flagDataToConsole", false).toBool();
    m_dateTimeInfo = m_settings.value("dateTimeInfo", true).toBool();
    m_dateYearInfo = m_settings.value("dateYearInfo", true).toBool();
    m_dateMonthInfo = m_settings.value("dateMonthInfo", true).toBool();
    m_dateDayInfo = m_settings.value("dateDayInfo", true).toBool();
    m_dateHourInfo = m_settings.value("dateHourInfo", true).toBool();
    m_dateMinuteInfo = m_settings.value("dateMinuteInfo", true).toBool();
    m_dateSecondInfo = m_settings.value("dateSecondInfo", true).toBool();
    m_copyDebugForChat = m_settings.value("copyDebugForChat", false).toBool();
    m_copySystemForChat = m_settings.value("copySystemForChat", false).toBool();
    m_streamChatOnOff = m_settings.value("streamChatOnOff", false).toBool();
    // цвета (список клиентов)
    m_colorGoPlaye = m_settings.value("colorGoPlaye", "#5fe915").toString();
    m_colorChat = m_settings.value("colorChat", "#fff015").toString();
    m_colorBusy = m_settings.value("colorBusy", "#ff0000").toString();
    // цвета (чат)
    m_colorFonFormInfo = m_settings.value("colorFonFormInfo", "#ffffff").toString();
    m_colorFonFormPrivat = m_settings.value("colorFonFormPrivat", "#ffffff").toString();
    m_colorFonFormGroup = m_settings.value("colorFonFormGroup", "#ffffff").toString();
    // цвет сообщений
    m_colorSystemInfo = m_settings.value("colorSystemInfo", "#555efd").toString();
    m_colordebug = m_settings.value("colordebug", "#000000").toString();
    m_colorChatPush = m_settings.value("colorChatPush", "#000000").toString();
    m_colorChatPull = m_settings.value("colorChatPull", "#000000").toString();
    m_colorChatPrivate = m_settings.value("colorChatPrivate","#de22de").toString();
    m_colorChatGroup = m_settings.value("colorChatGroup", "#bd8b00").toString();
    // шрифт (чат)
    m_systemInfoIndexFond = m_settings.value("systemInfoIndexFond", 0).toInt();
    m_debugIndexFond = m_settings.value("debugIndexFond", 0).toInt();
    m_chatPushIndexFond = m_settings.value("chatPushIndexFond", 0).toInt();
    m_chatPullIndexFond = m_settings.value("chatPullIndexFond", 0).toInt();
    m_chatPrivateIndexFond = m_settings.value("chatPrivateIndexFond", 0).toInt();
    m_chatGroupIndexFond = m_settings.value("chatGroupIndexFond", 0).toInt();
    // font
    m_systemInfoFont = m_settings.value("systemInfoFont", "").toString();
    m_debugFont = m_settings.value("debugFont", "").toString();
    m_chatPushFont = m_settings.value("chatPushFont", "").toString();
    m_chatPullFont = m_settings.value("chatPullFont", "").toString();
    m_chatPrivateFont = m_settings.value("chatPrivateFont", "").toString();
    m_chatGroupFont = m_settings.value("chatGroupFont", "").toString();
    // system (жир, курсив, черта)
    m_weightFontSystemInfo = m_settings.value("weightFontSystemInfo", false).toBool();
    m_italicFontSystemInfo = m_settings.value("italicFontSystemInfo", false).toBool();
    m_underlineFontSystemInfo = m_settings.value("underlineFontSystemInfo", false).toBool();
    // debug (жир, курсив, черта)
    m_weightFontDebug = m_settings.value("weightFontDebug", false).toBool();
    m_italicFontDebug = m_settings.value("italicFontDebug", false).toBool();
    m_underlineFontDebug = m_settings.value("underlineFontDebug", false).toBool();
    // push (жир, курсив, черта)
    m_weightFontchatPush = m_settings.value("weightFontchatPush", false).toBool();
    m_italicFontchatPush = m_settings.value("italicFontchatPush", false).toBool();
    m_underlineFontchatPush = m_settings.value("underlineFontchatPush", false).toBool();
    // pull (жир, курсив, черта)
    m_weightFontchatPull = m_settings.value("weightFontchatPull", false).toBool();
    m_italicFontchatPull = m_settings.value("italicFontchatPull", false).toBool();
    m_underlineFontchatPull = m_settings.value("underlineFontchatPull", false).toBool();
    // Private (жир, курсив, черта)
    m_weightFontchatPrivate = m_settings.value("weightFontchatPrivate", false).toBool();
    m_italicFontchatPrivate = m_settings.value("italicFontchatPrivate", false).toBool();
    m_underlineFontchatPrivate = m_settings.value("underlineFontchatPrivate", false).toBool();
    // Group (жир, курсив, черта)
    m_weightFontchatGroup = m_settings.value("weightFontchatGroup", false).toBool();
    m_italicFontchatGroup = m_settings.value("italicFontchatGroup", false).toBool();
    m_underlineFontchatGroup = m_settings.value("underlineFontchatGroup", false).toBool();
    // кнопки для чата
    m_clearChat = m_settings.value("clearChat", false).toBool();
    m_copyChat = m_settings.value("copyChat", false).toBool();
    m_saveFileChat = m_settings.value("saveFileChat", false).toBool();

    m_settings.endGroup();
}

void MyClient::writeConfig(QString path)
{
    QSettings m_settings(path, QSettings::IniFormat);
    m_settings.beginGroup("Settinhs");

    m_settings.setValue("strIPAddres", strIPAddres);
    m_settings.setValue("strLogun", strLogun);
    m_settings.setValue("port", m_port);
    m_settings.setValue("strPasswd", strPasswd);
    m_settings.setValue("totalChat", m_totalChat);
    m_settings.setValue("privatChat", m_privatChat);
    m_settings.setValue("infoWinChat",m_infoWinChat);
    m_settings.setValue("privatWinChat", m_privatWinChat);
    m_settings.setValue("groupWinChat", m_groupWinChat);
    m_settings.setValue("messagePrivateChat", m_messagePrivateChat);
    m_settings.setValue("messageGroupChat", m_messageGroupChat);
    m_settings.setValue("passwdEchoMode", passwdEchoMode);
    m_settings.setValue("spacingListGame", m_numSpacingListGame);
    m_settings.setValue("statusClient", m_statusClient);
    m_settings.setValue("debugToInfoLevel", m_debugToInfoLevel);
    m_settings.setValue("systemChatFlag", m_systemChatFlag);
    m_settings.setValue("logFileFlag", m_logFileFlag);
    m_settings.setValue("pathAndNameLogFile", m_pathAndNameLogFile);
    m_settings.setValue("flagDataToConsole", m_flagDataToConsole);
    m_settings.setValue("dateTimeInfo", m_dateTimeInfo);
    m_settings.setValue("dateYearInfo", m_dateYearInfo);
    m_settings.setValue("dateMonthInfo", m_dateMonthInfo);
    m_settings.setValue("dateDayInfo", m_dateDayInfo);
    m_settings.setValue("dateHourInfo", m_dateHourInfo);
    m_settings.setValue("dateMinuteInfo", m_dateMinuteInfo);
    m_settings.setValue("dateSecondInfo", m_dateSecondInfo);
    m_settings.setValue("copyDebugForChat", m_copyDebugForChat);
    m_settings.setValue("copySystemForChat", m_copySystemForChat);
    m_settings.setValue("streamChatOnOff", m_streamChatOnOff);
    // цвета (список клиентов)
    m_settings.setValue("colorGoPlaye", m_colorGoPlaye.name());
    m_settings.setValue("colorChat", m_colorChat.name());
    m_settings.setValue("colorBusy", m_colorBusy.name());
    // цвета (чат)
    m_settings.setValue("colorFonFormInfo", m_colorFonFormInfo.name());
    m_settings.setValue("colorFonFormPrivat", m_colorFonFormPrivat.name());
    m_settings.setValue("colorFonFormGroup", m_colorFonFormGroup.name());
    // цвет сообщения
    m_settings.setValue("colorSystemInfo", m_colorSystemInfo.name());
    m_settings.setValue("colordebug", m_colordebug.name());
    m_settings.setValue("colorChatPush", m_colorChatPush.name());
    m_settings.setValue("colorChatPull", m_colorChatPull.name());
    m_settings.setValue("colorChatPrivate", m_colorChatPrivate.name());
    m_settings.setValue("colorChatGroup", m_colorChatGroup.name());
    // шрифт (чат)
    m_settings.setValue("systemInfoIndexFond", m_systemInfoIndexFond);
    m_settings.setValue("debugIndexFond", m_debugIndexFond);
    m_settings.setValue("chatPushIndexFond", m_chatPushIndexFond);
    m_settings.setValue("chatPullIndexFond", m_chatPullIndexFond);
    m_settings.setValue("chatPrivateIndexFond", m_chatPrivateIndexFond);
    m_settings.setValue("chatGroupIndexFond", m_chatGroupIndexFond);
    // font
    m_settings.setValue("systemInfoFont", m_systemInfoFont.family());
    m_settings.setValue("debugFont", m_debugFont.family());
    m_settings.setValue("chatPushFont", m_chatPushFont.family());
    m_settings.setValue("chatPullFont", m_chatPullFont.family());
    m_settings.setValue("chatPrivateFont", m_chatPrivateFont.family());
    m_settings.setValue("chatGroupFont", m_chatGroupFont.family());
    // system (жир, курсив, черта)
    m_settings.setValue("weightFontSystemInfo", m_weightFontSystemInfo);
    m_settings.setValue("italicFontSystemInfo", m_italicFontSystemInfo);
    m_settings.setValue("underlineFontSystemInfo", m_underlineFontSystemInfo);
    // debug (жир, курсив, черта)
    m_settings.setValue("weightFontDebug", m_weightFontDebug);
    m_settings.setValue("italicFontDebug", m_italicFontDebug);
    m_settings.setValue("underlineFontDebug", m_underlineFontDebug);
    // push (жир, курсив, черта)
    m_settings.setValue("weightFontchatPush", m_weightFontchatPush);
    m_settings.setValue("italicFontchatPush", m_italicFontchatPush);
    m_settings.setValue("underlineFontchatPush", m_underlineFontchatPush);
    // pull (жир, курсив, черта)
    m_settings.setValue("weightFontchatPull", m_weightFontchatPull);
    m_settings.setValue("italicFontchatPull", m_italicFontchatPull);
    m_settings.setValue("underlineFontchatPull", m_underlineFontchatPull);
    // Private (жир, курсив, черта)
    m_settings.setValue("weightFontchatPrivate", m_weightFontchatPrivate);
    m_settings.setValue("italicFontchatPrivate", m_italicFontchatPrivate);
    m_settings.setValue("underlineFontchatPrivate", m_underlineFontchatPrivate);
    // Group (жир, курсив, черта)
    m_settings.setValue("weightFontchatGroup", m_weightFontchatGroup);
    m_settings.setValue("italicFontchatGroup", m_italicFontchatGroup);
    m_settings.setValue("underlineFontchatGroup", m_underlineFontchatGroup);
    // кнопки для чата
    m_settings.setValue("clearChat", m_clearChat);
    m_settings.setValue("copyChat", m_copyChat);
    m_settings.setValue("saveFileChat", m_saveFileChat);

    m_settings.endGroup();
    m_settings.sync();
}
// -----------------------------------------------------------



// -----------------------------------------------------------------
// кнопка закрыть
void MyClient::on_pb_close_clicked()
{
    sendToServer(comCloseClient);
    this->close();
}
// -----------------------------------------------------------------





// сброс данных
void MyClient::on_pb_break_clicked()
{
    ui->pb_break->setVisible(false);
    visibleGameKNB(false);
    ui->lb_statusFindGrup->setText("");
    vecOpponentKNB.clear();
    ui->lb_myOpponent->clear();
    sendToServer(comLeaveGroupsKNB);
    ui->hs_countPlayerKNB->setEnabled(true);
    ui->sb_countPlayerKNB->setEnabled(true);

    if (m_timerFormKNBStrart)       // если таймер запущен
    {
        stopTimerFindGroupsAndMotionKNB();
        stopTimerFindGroups();
    }

    clearDataClientGroup();
}


