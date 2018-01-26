#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"
#include "math.h"

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
QString validez, latitud, longitud, velocidad, pulsacion;
double m1, n1, m2, n2, m3, n3, m4, n4;  //esquinas mapeadas
QImage fondo;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    mThread = new DuThread(10,this);
    connect(mThread,&DuThread::valorCambiado, ui->progressBarserie, &QProgressBar::setValue);
    //----Serial----
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Pedir()));
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()){
            ui->serie_combo->insertItem(0,info.portName());
    }
    ui->serie_desconectar->setEnabled(false);
    //-----Plot----
    ui->plot->addGraph();
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCircle);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    fondo.load(":/futbol_pitch_green.png");
    //fondo.load(":/cancha_cesped.jpg");
    ui->plot->setBackground(fondo);
    //ui->plot->xAxis->setVisible(false);
    //ui->plot->yAxis->setVisible(false);
    //------SQL------
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
    ui->lineEditcorner1->setText("-31,747317, -60,514680");
    ui->lineEditcorner2->setText("-31,747910, -60,514836");
    ui->lineEditcorner3->setText("-31,747741, -60,515855");
    ui->lineEditcorner4->setText("-31,747162, -60,515723");
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
        double x = mostrar.value(3).toDouble(&ok);
        double y = mostrar.value(2).toDouble(&ok);
        addPoint(x, y);
        qDebug() << mostrar.value(3).toDouble(&ok) << mostrar.value(2).toDouble(&ok);
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
        latitud = QString::number(serial->read(10).append("e-2").toDouble(&ok),'f');
        longitud = QString::number(serial->read(11).append("e-2").toDouble(&ok),'f');
        velocidad = serial->read(5);
        pulsacion = serial->read(1);
        insertarUsuario();
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
//    ui->plot->xAxis->setVisible(false);
//    ui->plot->yAxis->setVisible(false);
}

void MainWindow::on_pushButtonmostrar_clicked()
{
    ui->lineEditcorner1->setText("-31,747307, -60,514661");
    ui->lineEditcorner2->setText("-31,747927, -60,514824");
    ui->lineEditcorner3->setText("-31,747754, -60,515868");
    ui->lineEditcorner4->setText("-31,747145, -60,515731");

    coordenadas(ui->lineEditcorner1->text(), ui->lineEditcorner2->text(), ui->lineEditcorner3->text(), ui->lineEditcorner4->text());
    //mostrarDatos();
    plot();
}

void MainWindow::coordenadas(QString esq1, QString esq2, QString esq3, QString esq4)
{
    //----corners----
    //-31,747159, -60,515733
    //separación de coordenadas reales
    esq1.replace(",", ".");
    esq2.replace(",", ".");
    esq3.replace(",", ".");
    esq4.replace(",", ".");
    double x1 = esq1.right(9).toDouble(&ok);
    esq1.chop(12);
    double y1 = esq1.right(9).toDouble(&ok);
    double x2 = esq2.right(9).toDouble(&ok);
    esq2.chop(12);
    double y2 = esq2.right(9).toDouble(&ok);
    double x3 = esq3.right(9).toDouble(&ok);
    esq3.chop(12);
    double y3 = esq3.right(9).toDouble(&ok);
    double x4 = esq4.right(9).toDouble(&ok);
    esq4.chop(12);
    double y4 = esq4.right(9).toDouble(&ok);
    //mapeo de las esquinas
//    m1 = x1;
//    n1 = y1;
//    m2 = m1;
//    n2 = sqrt(pow((x2-x1),2)+pow((y2-y1),2)) + n1;
//    m3 = sqrt(pow((x3-x2),2)+pow((y3-y2),2)) + m1;
//    n3 = n2;
//    m4 = m3;
//    n4 = n1;
    //disposición de los límites de los ejes
    ui->plot->xAxis->setRange(x1,x4);
    qDebug() << m1 << n1 << m2 << n2 << m3 << n3 << m4 << n4;
    ui->plot->yAxis->setRange(y1,y2);
    //mostrarDatos();
}
