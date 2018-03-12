#include "dialog_selectcourt.h"
#include "ui_dialog_selectcourt.h"

Dialog_SelectCourt::Dialog_SelectCourt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_SelectCourt)
{
    ui->setupUi(this);
    actualizarCanchas();
}

Dialog_SelectCourt::~Dialog_SelectCourt()
{
    delete ui;
}

void Dialog_SelectCourt::actualizarCanchas()
{
    file = new QFile("canchas.txt");
    if (!file->open(QIODevice::ReadOnly | QIODevice::Text))
        return;
    QTextStream in(file);
    QString linea;
    int index;
    int counter = 0;
    ui->comboBoxcancha->clear();
    while(!in.atEnd()){
        linea.clear();
        linea = in.readLine();
        index = linea.indexOf("*");
        linea = linea.left(index);
        listaCanchas.insert(1,linea);
        counter++;
    }
    foreach (const QString &str, listaCanchas)
        ui->comboBoxcancha->insertItem(1,str);
    listaCanchas.clear();
    file->close();
}

QString Dialog_SelectCourt::obtenerCancha()
{
    return seleccion;
}

void Dialog_SelectCourt::on_buttonBox_accepted()
{
    seleccion = ui->comboBoxcancha->currentText();
    emit senal();
}
