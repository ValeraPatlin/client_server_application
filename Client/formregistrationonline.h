#ifndef FORMREGISTRATIONONLINE_H
#define FORMREGISTRATIONONLINE_H

#include <QWidget>
#include "myclient.h"

class MyClient;
struct RegistrationClient;

namespace Ui {
class FormRegistrationOnline;
}

class FormRegistrationOnline : public QWidget
{
    Q_OBJECT

public:
    explicit FormRegistrationOnline( RegistrationClient regisClient, QWidget *parent = 0);
    ~FormRegistrationOnline();

private:
    Ui::FormRegistrationOnline *ui;
};

#endif // FORMREGISTRATIONONLINE_H
