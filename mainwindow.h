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
#include <QFile>
#include <dbmanager.h>
#include <dialog_nuevo.h>
#include <dialog_hrate.h>
#include <dialognewcourt.h>
#include <dialog_selectcourt.h>
#include <dialog_conexion.h>

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

    struct corners{
        QString nombreCancha;
        QString corner1;
        QString corner2;
        QString corner3;
        QString corner4;
    };
    //----Plot----
    void addPoint(double x, double y);
    void clearData();
    void plot();
    //------------
    void mostrarTabla();
    void mostrarDatos();

private slots:
    void openFile();
    void writeFile(QString texto);
    void readFile(QString palabra);
    void Serial_Conf(QString puertoNombre);
    void Serial_Conect();
    void Serial_Desconect();
    void Serial_Error();
    void Serial_Pedir();
    void on_pushButtonmostrar_clicked();
    void coordenadas(QString, QString, QString, QString);
    double Mapeo_x(double, double);
    double Mapeo_y(double, double);
    void on_pushButtonmanual_clicked();
    void on_pushButtonrandom_clicked();
    void on_verticalSlider_max_actionTriggered(int action);
    void on_action_Open_triggered();
    void ejecutarNuevo();
    void on_actionNew_triggered();
    void ejecutarNuevoJugador();
    void actualizarLista();
    void on_comboBoxtablas_activated(const QString &arg1);
    void on_actionAdd_Court_triggered();
    void agregarCancha();
    void on_actionShow_Position_triggered();
    void on_action_Seleccionar_Cancha_triggered();
    void seleccionCancha();
    void on_action_Configurar_Conexi_n_triggered();
    void setCom();

    void on_actionClose_triggered();

    void on_actionSerialConect_triggered();

    void on_actionserialConfig_triggered();

    void on_actionEquipo_triggered();

    void on_actionJugador_triggered();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    DuThread *mThread;
    QVector<double> qv_x, qv_y;
    Dialog_nuevo *dialogoNew;
    Dialog_HRate *dialogoHR;
    DialogNewCourt *dialogoNewCancha;
    Dialog_SelectCourt *dialogoSelectCourt;
    Dialog_Conexion *dialogoConexion;
    DbManager *db;
    QString path, nombre;
    QFile *file;
    QStringList listaCanchas;
    corners esquinas;
};

#endif // MAINWINDOW_H
