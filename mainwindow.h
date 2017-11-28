#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QMessageBox>
#include <iostream>
#include <QDebug>
#include <QSerialPortInfo>
#include <QVector>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void crearTablaUsuarios();
    void insertarUsuario();
    void mostrarDatos();

private slots:
    void Serial_Conf();

    void Serial_Conect();

    void Serial_Desconect();

    void Serial_Error();

    void Serial_Pedir();

    void on_serie_combo_activated(const QString &arg1);

    void on_serie_actualizar_clicked();

    void on_serie_desconectar_clicked();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QSqlDatabase db;
};

#endif // MAINWINDOW_H
