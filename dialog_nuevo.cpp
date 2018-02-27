#include "dialog_nuevo.h"
#include "ui_dialog_nuevo.h"

Dialog_nuevo::Dialog_nuevo(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_nuevo)
{
    ui->setupUi(this);
    ui->labelPhoto->setScaledContents(true);
    ui->labelPhoto->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
}

Dialog_nuevo::~Dialog_nuevo()
{
    delete ui;
}

QString Dialog_nuevo::obtenerNombre() const
{
    return player;
}

void Dialog_nuevo::ejecutarNuevo()
{
    QFile file(path);
    file.open(QIODevice::ReadOnly);
    currentPerfil.photo = file.readAll();
    QPixmap foto;
    foto.loadFromData(currentPerfil.photo);
    ui->labelPhoto->setPixmap(foto);
}

Dialog_nuevo::perfil Dialog_nuevo::obtenerPerfil()
{
    return currentPerfil;
}

void Dialog_nuevo::on_buttonBox_accepted()
{
    player = ui->lineEditnombre->text();
    currentPerfil.nombre = ui->lineEditnombre->text();
    currentPerfil.fecha = ui->dateEditNacimiento->date();
    currentPerfil.peso = ui->spinBoxPeso->value();
    currentPerfil.altura = ui->spinBoxAltura->value();
    emit senal();
}

void Dialog_nuevo::on_pushButtonPhoto_clicked()
{
    path = QFileDialog::getOpenFileName(
                this,
                "GPSport - Seleccionar una foto de perfil",
                "Users/javos/Documents/git/multiserie/",
                "Imágenes (*.jpg;*.jpeg;*.png);;All Files (*.*)");
    if(!path.isNull())
        ejecutarNuevo();
}
