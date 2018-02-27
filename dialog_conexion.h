#ifndef DIALOG_CONEXION_H
#define DIALOG_CONEXION_H

#include <QDialog>
#include <QSerialPortInfo>

namespace Ui {
class Dialog_Conexion;
}

class Dialog_Conexion : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Conexion(QWidget *parent = 0);
    ~Dialog_Conexion();
    QString obtenerPuerto();

signals:
    void senal();

private slots:
    void on_buttonBox_accepted();
    void on_pushButtonActualizar_clicked();
    void actualizar();

private:
    Ui::Dialog_Conexion *ui;
    QString puerto;
};

#endif // DIALOG_CONEXION_H
