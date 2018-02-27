#ifndef DIALOG_SELECTCOURT_H
#define DIALOG_SELECTCOURT_H

#include <QDialog>
#include <QFile>
#include <QTextStream>

namespace Ui {
class Dialog_SelectCourt;
}

class Dialog_SelectCourt : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_SelectCourt(QWidget *parent = 0);
    ~Dialog_SelectCourt();
    void actualizarCanchas();
    QString obtenerCancha();

signals:
    void senal();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_SelectCourt *ui;
    QFile *file;
    QStringList listaCanchas;
    QString seleccion;
};

#endif // DIALOG_SELECTCOURT_H
