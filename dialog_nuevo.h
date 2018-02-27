#ifndef DIALOG_NUEVO_H
#define DIALOG_NUEVO_H

#include <QDialog>
#include <QDebug>
#include <QPixmap>
#include <QFileDialog>
#include <QDate>

namespace Ui {
class Dialog_nuevo;
}

class Dialog_nuevo : public QDialog
{
    Q_OBJECT

public:
    struct perfil{
        QString nombre;
        QByteArray photo;
        int peso;
        int altura;
        QDate fecha;
    };

    explicit Dialog_nuevo(QWidget *parent = 0);
    ~Dialog_nuevo();
    QString obtenerNombre() const;
    void ejecutarNuevo();
    Dialog_nuevo::perfil obtenerPerfil();

signals:
    void senal();

private slots:
    void on_buttonBox_accepted();
    void on_pushButtonPhoto_clicked();

private:
    Ui::Dialog_nuevo *ui;
    QString player, path;
    perfil currentPerfil;
};

#endif // DIALOG_NUEVO_H
