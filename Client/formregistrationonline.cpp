#include "formregistrationonline.h"
#include "ui_formregistrationonline.h"

FormRegistrationOnline::FormRegistrationOnline(RegistrationClient regisClient, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FormRegistrationOnline)
{
    ui->setupUi(this);
}

FormRegistrationOnline::~FormRegistrationOnline()
{
    delete ui;
}
