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
#include <dialog_gps.h>

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
    struct Config{
        QString database;
        QString jugador;
        QString cancha;
        QString puerto;
    };
    //------------
    void mostrarTabla();
    void mostrarDatos();
    void mostrarFechas();
    //-----------
    Q_INVOKABLE bool nuevoJugador(const QString &nombre, const QString &fecha, const QString &altura, const QString &peso);
    Q_INVOKABLE QString cargarPhoto_clicked();
    Q_INVOKABLE bool buscarJugador(const QString &buscado);
    Q_INVOKABLE QStringList obtenerJugador();
    Q_INVOKABLE QStringList obtenerListaJugadores();
    Q_INVOKABLE void on_comboBoxtablas_activated(const QString &arg1);
    Q_INVOKABLE bool on_actionSerialConect_triggered();
    Q_INVOKABLE void on_actionNew_triggered();
    Q_INVOKABLE void on_actionMostrar_Analisis_triggered();
    Q_INVOKABLE QString obtenerPulsacionMaxima(int numero);
    Q_INVOKABLE QString obtenerPulsacionMinima(int numero);
    Q_INVOKABLE void calculoPrevioMaxMin();


signals:
    void serialConected();
    void serialDesconected();
    void validador(const QString validez);

private slots:
    void initConfiguration();
    void lastConfiguration();
    void openFile();
    void writeFile(QString texto);
    void readFile(QString palabra);
    void Serial_Conf(QString puertoNombre);
    void Serial_Conect();
    void Serial_Desconect();
    void Serial_Error();
    void Serial_Pedir();
    void filtroMatricial();
    void coordenadas(QString, QString, QString, QString);
    double Mapeo_x(double, double);
    double Mapeo_y(double, double);
    void on_pushButtonmanual_clicked();
    void on_pushButtonrandom_clicked();
    void on_action_Open_triggered();
    void ejecutarNuevo();
    void ejecutarNuevoJugador();
    void actualizarLista();
    void on_actionAdd_Court_triggered();
    void agregarCancha();
//    void on_actionMostrar_Analisis_triggered();
    void on_action_Seleccionar_Cancha_triggered();
    void seleccionCancha();
    void on_action_Configurar_Conexi_n_triggered();
    void setCom();
    void on_actionClose_triggered();
    void buscarFecha();

    void on_pushButton_clicked();

    void openDatabase();
    QString quitarEspacio(QString nombre);
    QString agregarEspacio(QString nombre);
    QDate formatoFecha(QString fecha);
    void pulsacionMaxMin(const QString &name);
    int serialCamiseta(const QString &player);
    QString nombreCamiseta(int camiseta);

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
    Dialog_Gps *dialogoGps;
    DbManager *db;
    QString path, nombre, pathFoto;
    QFile *file;
    QStringList listaCanchas;
    QStringList listaFechas;
    QStringList listaJugadores;
    corners esquinas;
    int ancho,alto;
    Config lastConfig;
    DbManager::PerfilBlock perfilBuscado, perfilNuevo;
    int maxPulso[11], minPulso[11];
};

#endif // MAINWINDOW_H
