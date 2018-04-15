#include <dbmanager.h>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>
#include <QDebug>

DbManager::DbManager(const QString &path)
{
    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(path);
    if (!m_db.open())
    {
        qDebug() << "Error: connection with database fail";
    }
    else
    {
        qDebug() << "Database: connection ok";
    }
}

DbManager::~DbManager()
{
    if (m_db.isOpen())
    {
        m_db.close();
        m_db.removeDatabase("QSQLITE");
    }
}

bool DbManager::isOpen() const
{
    return m_db.isOpen();
}

bool DbManager::createTable(QString tabla)
{
    bool success = false;
    QSqlQuery query;
    QString consulta;
    consulta.append("CREATE TABLE IF NOT EXISTS ");
    consulta.append(tabla);
    consulta.append("(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "validez VARCHAR(15),"
                    "latitud INTEGER NOT NULL,"
                    "longitud INTEGER NOT NULL,"
                    "velocidad INTEGER NOT NULL,"
                    "pulsacion INTEGER NOT NULL,"
                    "hora BLOB,"
                    "fecha BLOB);");
    query.prepare(consulta);
    if (!query.exec())
    {
        qDebug() << "Couldn't create the table "<< tabla <<": one might already exist.";
        success = false;
    }
    else
        success = true;
    return success;
}

bool DbManager::removeTable(QString tabla)
{
    bool success = false;
    QSqlQuery query;
    QString consulta;
    consulta.append("DROP TABLE IF EXISTS ");
    consulta.append(tabla);
    query.prepare(consulta);
    if (!query.exec())
    {
        qDebug() << "Couldn't remove the table "<< tabla;
        success = false;
    }
    return success;
}

bool DbManager::addData(QString tabla, DataBlock data)
{
    bool success = false;
    if (!tabla.isEmpty())
    {
        QString pedido;
        pedido.append("INSERT INTO ");
        pedido.append(tabla);
        pedido.append(" (validez,latitud,longitud,velocidad,pulsacion,hora,fecha) "
                      "VALUES (:validez,:latitud,:longitud,:velocidad,:pulsacion,:hora,:fecha)");
        QSqlQuery queryAdd;
        queryAdd.prepare(pedido);
        queryAdd.bindValue(":validez", data.validez);
        queryAdd.bindValue(":latitud", data.latitud);
        queryAdd.bindValue(":longitud", data.longitud);
        queryAdd.bindValue(":velocidad", data.velocidad);
        queryAdd.bindValue(":pulsacion", data.pulsacion);
        queryAdd.bindValue(":hora", data.hora);
        queryAdd.bindValue(":fecha", data.fecha);
        if(queryAdd.exec())
        {
            success = true;
        }
        else
        {
            qDebug() << "add player failed: " << queryAdd.lastError();
        }
    }
    else
    {
        qDebug() << "add player failed: name cannot be empty";
    }
    return success;
}

QStringList DbManager::obtenerLista()
{
      return m_db.tables();
}

void DbManager::printPlayer(QString tabla) const
{
    QString pedido;
    pedido.append("SELECT * FROM ");
    pedido.append(tabla);
    QSqlQuery query(pedido);
    int idName = query.record().indexOf("name");
    while (query.next())
    {
        QString name = query.value(idName).toString();
        qDebug() << "===" << name;
    }
}

bool DbManager::personExists(const QString& name) const
{
    bool exists = false;

    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT name FROM people WHERE name = (:name)");
    checkQuery.bindValue(":name", name);

    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
        }
    }
    else
    {
        qDebug() << "person exists failed: " << checkQuery.lastError();
    }

    return exists;
}

bool DbManager::removeAllPersons()
{
    bool success = false;

    QSqlQuery removeQuery;
    removeQuery.prepare("DELETE FROM people");

    if (removeQuery.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "remove all persons failed: " << removeQuery.lastError();
    }

    return success;
}

bool DbManager::createTablePerfiles()
{
    bool success = false;
    QSqlQuery query;
    QString consulta;
    consulta.append("CREATE TABLE IF NOT EXISTS perfiles");
    consulta.append("(id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "nombre VARCHAR(15),"
                    "photo LONGBLOB,"
                    "fecha BLOB,"
                    "altura INTEGER NOT NULL,"
                    "peso INTEGER NOT NULL,"
                    "maxppm INTEGER NOT NULL);");
    query.prepare(consulta);
    if (!query.exec())
    {
        qDebug() << "Couldn't create the table perfiles: one might already exist.";
        success = false;
    }
    else
        success = true;
    return success;
}

bool DbManager::addPerfil(PerfilBlock data)
{
    bool success = false;
    QString pedido;
    int dia, mes, ano;
    data.fecha.getDate(&ano,&mes,&dia);
    pedido.append("INSERT INTO perfiles");
    pedido.append(" (nombre,photo,fecha,altura,peso,maxppm) "
                  "VALUES (:nombre,:photo,:fecha,:altura,:peso,:maxppm)");
    QSqlQuery queryAdd;
    queryAdd.prepare(pedido);
    queryAdd.bindValue(":nombre", data.nombre);
    queryAdd.bindValue(":photo", data.photo);
    queryAdd.bindValue(":fecha", data.fecha);
    queryAdd.bindValue(":altura",data.altura);
    queryAdd.bindValue(":peso",data.peso);
    queryAdd.bindValue(":maxppm",data.maxppm);
    if(queryAdd.exec())
    {
        success = true;
    }
    else
    {
        qDebug() << "add player failed: " << queryAdd.lastError();
    }
    return success;
}

DbManager::PerfilBlock DbManager::obtenerPerfil() const
{
    return perfilBuscado;
}

bool DbManager::buscarPerfil(const QString& name)
{
    bool exists = false;
    bool ok;
    int dia, mes, ano;
    QSqlQuery checkQuery;
    checkQuery.prepare("SELECT id, nombre, photo, fecha, peso, altura, maxppm FROM perfiles WHERE nombre = (:name)");
    checkQuery.bindValue(":name", name);
    if (checkQuery.exec())
    {
        if (checkQuery.next())
        {
            exists = true;
            perfilBuscado.nombre = checkQuery.value(1).toString();
            perfilBuscado.photo = checkQuery.value(2).toByteArray().toBase64();
            perfilBuscado.fecha = checkQuery.value(3).toDate();
            perfilBuscado.altura = checkQuery.value(4).toInt(&ok);
            perfilBuscado.peso = checkQuery.value(5).toInt(&ok);
            perfilBuscado.maxppm = checkQuery.value(6).toInt(&ok);
        }
    }
    else
    {
        qDebug() << "perfil exists failed: " << checkQuery.lastError();
    }
    return exists;
}
