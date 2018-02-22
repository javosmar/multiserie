#ifndef DIALOG_NUEVO_H
#define DIALOG_NUEVO_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class Dialog_nuevo;
}

class Dialog_nuevo : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog_nuevo(QWidget *parent = 0);
    ~Dialog_nuevo();
    QString obtenerNombre() const;

signals:
    void senal();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::Dialog_nuevo *ui;
    QString player;
};

#endif // DIALOG_NUEVO_H
