#ifndef DIALOG_GPS_H
#define DIALOG_GPS_H

#include <QDialog>
#include <qcustomplot.h>

namespace Ui {
class Dialog_Gps;
}

class Dialog_Gps : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_Gps(QWidget *parent = 0);
    ~Dialog_Gps();

    void plot(float vector[][64],int);

private slots:
    void on_pushButtonCerrar_clicked();

private:
    Ui::Dialog_Gps *ui;
    QCPColorScale *colorScale;
    double max = 0;
};

#endif // DIALOG_GPS_H
