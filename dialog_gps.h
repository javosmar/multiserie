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
};

#endif // DIALOG_GPS_H
