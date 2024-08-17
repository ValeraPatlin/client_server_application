#include "myserver.h"
#include <QApplication>

//---------------------------------------------------------------------------------
// logger Для логирования
#include <QLoggingCategory>
#include <QTextStream>
#include <QDateTime>

QFile log_file;
int debuglevel{};

void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    //if (debuglevel == 8)
    {
        QTextStream out(stdout);
        out << QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss--");
        out << context.category << " : " << msg << "\n";
        out.flush();
    }
}
// --------------------------------------------------------------------------------------


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MyServer server(2323);
    server.show();

// --------------------------------------------------------------------------------
//log   Для логирования, логер файл перезаписывается
    log_file.setFileName("log.txt");
    log_file.open(QIODevice::WriteOnly);
    //qInstallMessageHandler(messageHandler);
// ---------------------------------------------------------------------------------


    return app.exec();
}
