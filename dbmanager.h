#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>

class DbManager
{
public:
    struct DataBlock{
        QString validez;
        int latitud;
        int longitud;
        int velocidad;
        QString pulsacion;
    };

    DbManager(const QString& path);
    ~DbManager();
    bool isOpen() const;
    bool createTable(QString);
    bool removeTable(QString);
    bool addData(QString, DataBlock);
    QStringList obtenerLista();
    void printPlayer(QString) const;

    bool personExists(const QString& name) const;
    bool removeAllPersons();

private:
    QSqlDatabase m_db;
};

#endif // DBMANAGER_H
