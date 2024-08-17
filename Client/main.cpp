#include "myclient.h"
#include <QApplication>
#include <QLoggingCategory>
#include <QTextStream>
#include <QFile>
#include <QIODevice>


//------------------------------------------
// logger Для логирования
QFile log_file;
bool log_good{};

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg);
// -----------------------------------------



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyClient client;

    //  установка стиля CSS
    QFile fileStyle(":/style/Style/Style.css");
    fileStyle.open(QFile::ReadOnly);

    //app.setStyle("FlyPlastique");
    //app.setStyleSheet(fileStyle.readAll());

    client.show();

// --------------------------------------------------------------------------------
//log   Для логирования, логер файл перезаписывается

    bool flagLogFile = client.getFlagLogFile();
    QString strPathLogFile = client.getPathLogFile();

    if (flagLogFile && strPathLogFile != "")
    {
        log_file.setFileName(strPathLogFile);
        log_file.open(QIODevice::WriteOnly);
        log_good = true;
    }
    else
    {
        log_good = false;
    }

    qInstallMessageHandler(messageHandler);
// ---------------------------------------------------------------------------------

    return app.exec();
}



void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    //QTextStream out;


    if (log_good)
    {
        //out.setDevice(&log_file);


        QTextStream out(&log_file);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss--");
        out << context.category << " : " << msg << "\n";
        out.flush();
    }
    else
    {
        //out.setDevice(stdout);


        QTextStream out(stdout);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss--");
        out << context.category << " : " << msg << "\n";
        out.flush();
    }
}
