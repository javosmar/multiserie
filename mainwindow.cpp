#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
QString validez, latitud, longitud, velocidad, pulsacion;

QImage fondo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mThread = new DuThread(10,this);
    connect(mThread,&DuThread::valorCambiado, ui->progressBarserie, &QProgressBar::setValue);


    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Pedir()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->serie_combo->insertItem(0,info.portName());
    }
    ui->serie_desconectar->setEnabled(false);

    //-----Plot----
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssDisc);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);

    fondo.load(":/futbol_pitch_green.png");
    ui->plot->setBackground(fondo);

    //ui->plot->setWindowOpacity(0.0);
    ui->plot->xAxis->setVisible(false);
    ui->plot->yAxis->setVisible(false);
    ui->plot->xAxis->setRange(3144887,3144935);
    ui->plot->yAxis->setRange(6030912,6030966);
    ui->doubleSpinBox_xmin->setValue(3144887);
    ui->doubleSpinBox_xmax->setValue(3144935);
    ui->doubleSpinBox_ymin->setValue(6030912);
    ui->doubleSpinBox_ymax->setValue(6030966);
    //------SQL------
    qDebug() << "Iniciado";
    QString nombre;
    nombre.append("base_datos.sqlite");
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(nombre);
    if(db.open()){
        qDebug() << "base de datos iniciada";
    }
    else{
        qDebug() << "error al abrir base de datos";
    }
    crearTablaUsuarios();
    mostrarDatos();
    //--------------
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::crearTablaUsuarios()
{
    QString consulta;
    consulta.append("CREATE TABLE IF NOT EXISTS jugador("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "validez VARCHAR(15),"
                    "latitud INTEGER NOT NULL,"
                    "longitud INTEGER NOT NULL,"
                    "velocidad INTEGER NOT NULL,"
                    "pulsacion VARCHAR(15)"
                    ");");
    QSqlQuery crear;
    crear.prepare(consulta);
    if(crear.exec()){
        qDebug() << "tabla creada correctamente";
    }
    else{
        qDebug() << "ERROR! " << crear.lastError();
    }
}

void MainWindow::insertarUsuario()
{
    QString consulta;
    consulta.append("INSERT INTO jugador("
                    "validez,"
                    "latitud,"
                    "longitud,"
                    "velocidad,"
                    "pulsacion) "
                    "VALUES("
                    "'"+validez+"',"
                    ""+latitud+","
                    ""+longitud+","
                    ""+velocidad+","
                    "'"+pulsacion+"'"
                    ");");
    QSqlQuery insertar;
    insertar.prepare(consulta);
    if(insertar.exec()){
        //qDebug() << "usuario agregado correctamente";
    }
    else{
        //qDebug() << "ERROR! " << insertar.lastError();
    }
}

void MainWindow::mostrarDatos()
{
    QString consulta;
    consulta.append("SELECT * FROM jugador");
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(mostrar.exec()){
        //qDebug() << "consulta realizada con exito";
    }
    else{
        qDebug() << "ERROR! " << mostrar.lastError();
    }
    int fila = 0;
    ui->tableWidgetdato->setRowCount(0);
    while(mostrar.next()){
        ui->tableWidgetdato->insertRow(fila);
        ui->tableWidgetdato->setItem(fila,0,new QTableWidgetItem(mostrar.value(1).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,1,new QTableWidgetItem(mostrar.value(2).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,2,new QTableWidgetItem(mostrar.value(3).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,3,new QTableWidgetItem(mostrar.value(4).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,4,new QTableWidgetItem(mostrar.value(5).toByteArray().constData()));
        addPoint(mostrar.value(2).toFloat(&ok), mostrar.value(3).toDouble(&ok));
        fila++;
    }
    plot();
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
        serial->write("A");
        mThread->start(QThread::LowestPriority);
    }
    else{
        Serial_Error();
    }
}

void MainWindow::Serial_Desconect()
{
    serial->write("X");
    serial->waitForBytesWritten(30);
    serial->close();
    estado_serial = false;
    ui->serie_combo->setEnabled(true);
    ui->serie_actualizar->setEnabled(true);
    ui->serie_desconectar->setEnabled(false);
    mThread->terminate();
}

void MainWindow::Serial_Error()
{
    QMessageBox error;
    error.setText("Verifique la conexión de la placa.");
    error.setIcon(QMessageBox::Warning);
    error.exec();
}

void MainWindow::Serial_Pedir()
{
    if(serial->bytesAvailable() >= 28){
        validez = serial->read(1);
        latitud = QString::number(serial->read(8).append("e+3").toDouble(&ok),'f');
        QString perdido = serial->read(2);
        longitud = QString::number(serial->read(9).append("e+3").toDouble(&ok),'f');
        perdido = serial->read(2);
        velocidad = serial->read(5);
        pulsacion = serial->read(1);
        insertarUsuario();
        //mostrarDatos();
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
    mostrarDatos();
}

void MainWindow::addPoint(double x, double y)
{
    qv_x.append(x);
    qv_y.append(y);
}

void MainWindow::clearData()
{
    qv_x.clear();
    qv_y.clear();
}

void MainWindow::plot()
{
    ui->plot->graph(0)->setData(qv_x, qv_y);
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::on_pushButton_clicked()
{
    mostrarDatos();
    ui->plot->xAxis->setRange(ui->doubleSpinBox_xmin->value(), ui->doubleSpinBox_xmax->value());
    ui->plot->yAxis->setRange(ui->doubleSpinBox_ymin->value(), ui->doubleSpinBox_ymax->value());
    plot();
}
