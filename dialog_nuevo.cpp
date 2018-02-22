#include "dialog_nuevo.h"
#include "ui_dialog_nuevo.h"

Dialog_nuevo::Dialog_nuevo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_nuevo)
{
    ui->setupUi(this);
}

Dialog_nuevo::~Dialog_nuevo()
{
    delete ui;
}

QString Dialog_nuevo::obtenerNombre() const
{
    return player;
}

void Dialog_nuevo::on_buttonBox_accepted()
{
    player = ui->lineEditnombre->text();
    emit senal();
}
