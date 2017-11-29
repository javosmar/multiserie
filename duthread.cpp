#include "duthread.h"

DuThread::DuThread(int tiempo, QObject *parent) : QThread(parent)
{
    mTiempo = tiempo;
}

void DuThread::run()
{
    int contador = 0;
    while(true){
        if(contador == 100){
            contador = 0;
        }
        emit valorCambiado(contador);
        msleep(mTiempo);
        contador++;
    }
}
