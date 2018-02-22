#include "dialognewcourt.h"
#include "ui_dialognewcourt.h"

DialogNewCourt::DialogNewCourt(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewCourt)
{
    ui->setupUi(this);
    clipBoard = QApplication::clipboard();
    ui->pushButtonAceptar->setEnabled(false);
    ui->lineEditcorner1->setEnabled(false);
    ui->lineEditcorner2->setEnabled(false);
    ui->lineEditcorner3->setEnabled(false);
    ui->lineEditcorner4->setEnabled(false);
}

DialogNewCourt::~DialogNewCourt()
{
    delete ui;
}

QString DialogNewCourt::obtenerCancha()
{
    return cancha;
}

void DialogNewCourt::limpiarCancha()
{
    cancha.clear();
    ui->lineEditcancha->clear();
    ui->lineEditcorner1->clear();
    ui->lineEditcorner2->clear();
    ui->lineEditcorner3->clear();
    ui->lineEditcorner4->clear();
    ui->pushButtonAceptar->setEnabled(false);
}

void DialogNewCourt::on_pushButtonMaps_clicked()
{
    connect(clipBoard,SIGNAL(changed(QClipboard::Mode)),this,SLOT(copiaDatos()));
    connect(ui->lineEditcorner4,SIGNAL(textChanged(QString)),this,SLOT(modificado()));
    contador = 0;
    QDesktopServices::openUrl(QUrl("https://www.bing.com/maps", QUrl::TolerantMode));
}

void DialogNewCourt::copiaDatos()
{
    contador++;
    switch(contador){
        case 1:
            ui->lineEditcorner1->setText(clipBoard->text());
            break;
        case 2:
            ui->lineEditcorner2->setText(clipBoard->text());
            break;
        case 3:
            ui->lineEditcorner3->setText(clipBoard->text());
            break;
        case 4:
            ui->lineEditcorner4->setText(clipBoard->text());
            break;
    }
    if(contador > 3){
        contador = 0;
        disconnect(clipBoard,0,this,0);
    }
}

void DialogNewCourt::on_pushButtonAceptar_clicked()
{
    cancha.append(ui->lineEditcancha->text());
    cancha.append("*");
    cancha.append(ui->lineEditcorner1->text());
    cancha.append("*");
    cancha.append(ui->lineEditcorner2->text());
    cancha.append("*");
    cancha.append(ui->lineEditcorner3->text());
    cancha.append("*");
    cancha.append(ui->lineEditcorner4->text());
    emit senal();
    this->close();
}

void DialogNewCourt::on_pushButtonCancelar_clicked()
{
    ui->lineEditcancha->clear();
    ui->lineEditcorner1->clear();
    ui->lineEditcorner2->clear();
    ui->lineEditcorner3->clear();
    ui->lineEditcorner4->clear();
    ui->pushButtonAceptar->setEnabled(false);
    this->close();
}

void DialogNewCourt::modificado()
{
    disconnect(ui->lineEditcorner4,0,this,0);
    ui->pushButtonAceptar->setEnabled(true);
}
