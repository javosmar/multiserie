#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QSqlDatabase>
#include <QDate>
#include <QPixmap>

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

    struct PerfilBlock{
        QString nombre;
        QByteArray photo;
        int peso;
        int altura;
        QDate fecha;
    };

    DbManager(const QString& path);
    ~DbManager();
    bool isOpen() const;
    bool createTable(QString);
    bool removeTable(QString);
    bool addData(QString, DataBlock);
    QStringList obtenerLista();
    void printPlayer(QString) const;

    bool createTablePerfiles();
    bool addPerfil(PerfilBlock data);
    bool buscarPerfil(const QString &name);
    PerfilBlock obtenerPerfil() const;

    bool personExists(const QString& name) const;
    bool removeAllPersons();

private:
    QSqlDatabase m_db;
    PerfilBlock perfilBuscado;
};

#endif // DBMANAGER_H
