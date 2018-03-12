#ifndef DIALOG_GPS_H
#define DIALOG_GPS_H

#include <QDialog>
#include <qcustomplot.h>

#define col 110
#define fil 64
#define div 5

namespace Ui {
class Dialog_Gps;
}

class Dialog_Gps : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Gps(QWidget *parent = 0);
    ~Dialog_Gps();

    void plot(float vector[][fil],int);
    void setListaFechas(QStringList);
    QDate obtenerFecha();
    void setGrafPulsos();
    void setTiempoPulso(QTime time, int pulse);
    void clearData();
    void initGrafPulsos();
    void setNombre(const QString &name);
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
};

#endif // DIALOG_GPS_H
