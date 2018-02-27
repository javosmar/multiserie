#ifndef DUTHREAD_H
#define DUTHREAD_H

#include <QThread>

class DuThread : public QThread
{
    Q_OBJECT
public:
    DuThread(int tiempo, QObject *parent = nullptr);

signals:
    void valorCambiado(int);
    void actualizar();

protected:
    void run() override;

private:
    int mTiempo;
};

#endif // DUTHREAD_H
