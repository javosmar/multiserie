#ifndef DIALOGNEWCOURT_H
#define DIALOGNEWCOURT_H

#include <QDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QClipboard>
#include <QDebug>

namespace Ui {
class DialogNewCourt;
}

class DialogNewCourt : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewCourt(QWidget *parent = 0);
    ~DialogNewCourt();

    QString obtenerCancha();
    void limpiarCancha();

signals:
    void senal();

private slots:
    void on_pushButtonMaps_clicked();
    void copiaDatos();
    void on_pushButtonAceptar_clicked();
    void on_pushButtonCancelar_clicked();
    void modificado();

private:
    Ui::DialogNewCourt *ui;
    QString cancha;
    QClipboard *clipBoard;
    int contador;
};

#endif // DIALOGNEWCOURT_H
