#ifndef DIALOG_GPS_H
#define DIALOG_GPS_H

#include <QDialog>
#include <qcustomplot.h>

#define col 118     //97  //110   #define col 95  95  95
#define fil 82    //70  //64    #define fil 73  76  65
#define div 5.1   //6.3 //      #define div 6.5 6.5 6.5

namespace Ui {
class Dialog_Gps;
}

class Dialog_Gps : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Gps(QWidget *parent = 0);
    ~Dialog_Gps();

    void plot(float vector[][fil], int columnas);
    void setListaFechas(QStringList);
    QDate obtenerFecha();
    void setGrafPulsos();
    void setTiempoPulso(QTime time, int pulse, float velocidad);
    void clearData();
    void initGrafPulsos();
    void setNombre(const QString &name, const int &maxppm);
    void setDatos(const float &maxVel,const int &maxPul,const int &minPul,const int &promPul);

signals:
    void senal();

private slots:
    void on_pushButtonCerrar_clicked();
    void on_comboBoxFecha_activated(const QString &arg1);

private:
    Ui::Dialog_Gps *ui;
    QCPColorScale *colorScale;
    double max = 0;
    QDate selectedFecha;
    int i;
    QVector<QTime> tiempo;
    QVector<double> pulsos;
    QVector<double> velocidades;
    float vMax;
    int fcmax;
};

#endif // DIALOG_GPS_H
