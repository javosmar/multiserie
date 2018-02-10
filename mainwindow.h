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
#include <QColormap>

namespace Ui {
class MainWindow;
}

class DuThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    //----Plot----
    void addPoint(double x, double y);
    void clearData();
    void plot();
    //------------

    void crearTablaUsuarios();
    void insertarUsuario();
    void mostrarTabla();
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

    void on_pushButtonmostrar_clicked();

    void coordenadas(QString, QString, QString, QString);

    double Mapeo_x(double, double);

    double Mapeo_y(double, double);

    void on_pushButton_clicked();

    void contador(void);

    void on_pushButton_2_clicked();

    void on_verticalSlider_max_actionTriggered(int action);

    void on_action_Open_triggered();

    void on_actionNew_triggered();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QSqlDatabase db;
    DuThread *mThread;
    QVector<double> qv_x, qv_y;
};

#endif // MAINWINDOW_H
