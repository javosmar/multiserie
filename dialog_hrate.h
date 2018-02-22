#ifndef DIALOG_HRATE_H
#define DIALOG_HRATE_H

#include <QDialog>

namespace Ui {
class Dialog_HRate;
}

class Dialog_HRate : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_HRate(QWidget *parent = 0);
    ~Dialog_HRate();

    void setHrPlayer(QString player, int ritmo);

signals:
    void senal();

private slots:
    void on_pushButtonDesconectar_clicked();

private:
    Ui::Dialog_HRate *ui;
};

#endif // DIALOG_HRATE_H
