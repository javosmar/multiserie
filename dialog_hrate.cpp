#include "dialog_hrate.h"
#include "ui_dialog_hrate.h"

Dialog_HRate::Dialog_HRate(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_HRate)
{
    ui->setupUi(this);
    ui->checkBoxOnline->setChecked(true);
    this->setWindowTitle("GPSport - Monitor Cardíaco");
}

Dialog_HRate::~Dialog_HRate()
{
    delete ui;
}

void Dialog_HRate::on_pushButtonDesconectar_clicked()
{
    emit senal();
    this->hide();
}

void Dialog_HRate::setHrPlayer(QString player, int ritmo)
{
    int camiseta;
    if(player == 'A')
        camiseta = 1;
    else if(player == 'B')
        camiseta = 2;
    else if(player == 'C')
        camiseta = 3;
    else if(player == 'D')
        camiseta = 4;
    else if(player == 'E')
        camiseta = 5;
    else if(player == 'F')
        camiseta = 6;
    else if(player == 'G')
        camiseta = 7;
    else if(player == 'H')
        camiseta = 8;
    else if(player == 'I')
        camiseta = 9;
    else if(player == 'J')
        camiseta = 10;
    else if(player == 'K')
        camiseta = 11;
    switch(camiseta){
        case 1:
            ui->labelCardio->setText(QString::number(ritmo));
            break;
        case 2:
            ui->labelCardio_2->setText(QString::number(ritmo));
            break;
        case 3:
            ui->labelCardio_3->setText(QString::number(ritmo));
            break;
        case 4:
            ui->labelCardio_4->setText(QString::number(ritmo));
            break;
        case 5:
            ui->labelCardio_5->setText(QString::number(ritmo));
            break;
        case 6:
            ui->labelCardio_6->setText(QString::number(ritmo));
            break;
        case 7:
            ui->labelCardio_7->setText(QString::number(ritmo));
            break;
        case 8:
            ui->labelCardio_8->setText(QString::number(ritmo));
            break;
        case 9:
            ui->labelCardio_9->setText(QString::number(ritmo));
            break;
        case 10:
            ui->labelCardio_10->setText(QString::number(ritmo));
            break;
        case 11:
            ui->labelCardio_11->setText(QString::number(ritmo));
            break;
        default:
            break;
    }
}
