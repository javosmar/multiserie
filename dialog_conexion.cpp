#include "dialog_conexion.h"
#include "ui_dialog_conexion.h"

Dialog_Conexion::Dialog_Conexion(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Conexion)
{
    ui->setupUi(this);
    actualizar();
}

Dialog_Conexion::~Dialog_Conexion()
{
    delete ui;
}

QString Dialog_Conexion::obtenerPuerto()
{
    return puerto;
}

void Dialog_Conexion::on_buttonBox_accepted()
{
    puerto =  ui->comboBoxPuerto->currentText();
    emit senal();
}

void Dialog_Conexion::on_pushButtonActualizar_clicked()
{
    actualizar();
}

void Dialog_Conexion::actualizar()
{
    ui->comboBoxPuerto->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
        ui->comboBoxPuerto->insertItem(1,info.portName());
}
