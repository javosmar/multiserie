#include "mainwindow.h"
#include "ui_mainwindow.h"

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false;
int x = 0;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Pedir()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->serie_combo->insertItem(0,info.portName());
    }
    ui->serie_desconectar->setEnabled(false);
//    //------SQL------
//    qDebug() << "Iniciado";
//    QString nombre;
//    nombre.append("base_datos.sqlite");
//    db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setDatabaseName(nombre);
//    if(db.open()){
//        qDebug() << "base de datos iniciada";
//    }
//    else{
//        qDebug() << "error al abrir base de datos";
//    }
//    crearTablaUsuarios();
//    mostrarDatos();
//    //--------------
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::crearTablaUsuarios()
{
//    QString consulta;
//    consulta.append("CREATE TABLE IF NOT EXISTS jugador("
//                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
//                    "valido VARCHAR(10),"
//                    "latitud INTEGER NOT NULL,"
//                    "longitud INTEGER NOT NULL,"
//                    "velocidad INTEGER NOT NULL,"
//                    "pulsacion INTEGER NOT NULL"
//                    ");");
//    QSqlQuery crear;
//    crear.prepare(consulta);
//    if(crear.exec()){
//        qDebug() << "tabla creada correctamente";
//    }
//    else{
//        qDebug() << "ERROR! " << crear.lastError();
//    }
}

void MainWindow::insertarUsuario()
{
//    QString consulta;
//    consulta.append("INSERT INTO jugador("
//                    "validez,"
//                    "latitud,"
//                    "longitud,"
//                    "velocidad,"
//                    "pulsacion)"
//                    "VALUES("
//                    "'"+ui->lineEditnombre->text()+"',"
//                    ""+ui->lineEditapellido->text()+","
//                    ""+ui->lineEditedad->text()+","
//                    ""+ui->lineEditedad->text()+","
//                    ""+ui->lineEditclase->text()+""
//                    ");");
//    QSqlQuery insertar;
//    insertar.prepare(consulta);
//    if(insertar.exec()){
//        qDebug() << "usuario agregado correctamente";
//    }
//    else{
//        qDebug() << "ERROR! " << insertar.lastError();
//    }
}

void MainWindow::mostrarDatos()
{
//    QString consulta;
//    consulta.append("SELECT * FROM jugador");
//    QSqlQuery mostrar;
//    mostrar.prepare(consulta);
//    if(mostrar.exec()){
//        qDebug() << "consulta realizada con exito";
//    }
//    else{
//        qDebug() << "ERROR! " << mostrar.lastError();
//    }

//    int fila = 0;
//    ui->tableWidgetdato->setRowCount(0);
//    while(mostrar.next()){
//        ui->tableWidgetdato->insertRow(fila);
//        ui->tableWidgetdato->setItem(fila,0,new QTableWidgetItem(mostrar.value(1).toByteArray().constData()));
//        ui->tableWidgetdato->setItem(fila,1,new QTableWidgetItem(mostrar.value(2).toByteArray().constData()));
//        ui->tableWidgetdato->setItem(fila,2,new QTableWidgetItem(mostrar.value(3).toByteArray().constData()));
//        ui->tableWidgetdato->setItem(fila,3,new QTableWidgetItem(mostrar.value(4).toByteArray().constData()));
//        fila++;
//    }
}

void MainWindow::Serial_Conf()
{
    serial->setPortName(ui->serie_combo->currentText());
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
}

void MainWindow::Serial_Conect()
{
    if((serial->open(QIODevice::ReadWrite)))
    {
        estado_serial = true;
        ui->serie_combo->setEnabled(false);
        ui->serie_actualizar->setEnabled(false);
        ui->serie_desconectar->setEnabled(true);
    }
    else
        Serial_Error();
}

void MainWindow::Serial_Desconect()
{
    serial->close();
    estado_serial = false;
    ui->serie_combo->setEnabled(true);
    ui->serie_actualizar->setEnabled(true);
    ui->serie_desconectar->setEnabled(false);
}

void MainWindow::Serial_Error()
{
    QMessageBox error;
    error.setText("Verifique la conexión de la placa.");
    error.setIcon(QMessageBox::Warning);
    error.exec();
}

void MainWindow::Serial_Pedir()                         //muestreo de datos por pedido
{
    if(serial->bytesAvailable() >= 28){
        QString validez = serial->read(1);
        QString latitud = serial->read(10);
        QString longitud = serial->read(11);
        QString velocidad = serial->read(5);
        QString pulsacion = serial->read(1);
        qDebug() << validez;
        qDebug() << latitud;
        qDebug() << longitud;
        qDebug() << velocidad;
        qDebug() << pulsacion;
    }
}

void MainWindow::on_serie_actualizar_clicked()
{
    ui->serie_combo->clear();
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->serie_combo->insertItem(0,info.portName());
    }
}

void MainWindow::on_serie_combo_activated(const QString &arg1)
{
    Serial_Conf();
    Serial_Conect();

}

void MainWindow::on_serie_desconectar_clicked()
{
    Serial_Desconect();
}
