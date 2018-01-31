#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"
#include "math.h"

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
QString validez, latitud, longitud, velocidad, pulsacion;
double fi_rad, alfa_rad;
int m1, n1, m2, n2, m3, n3, m4, n4, m5, n5, m6, n6, m7, n7, m8, n8, xprima, yprima, X1, Y1;  //esquinas mapeadas
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
    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);//::ssCircle);
    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
    fondo.load(":/futbol_pitch_green.png");
    ui->plot->setBackground(fondo);
    ui->plot->xAxis->setVisible(false);
    ui->plot->yAxis->setVisible(false);
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
    ui->lineEditcorner1->setText("-31,747308, -60,514664");
    ui->lineEditcorner2->setText("-31,747931, -60,514819");
    ui->lineEditcorner3->setText("-31,747757, -60,515908");
    ui->lineEditcorner4->setText("-31,747135, -60,515763");
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
        int x = mostrar.value(3).toInt(&ok);
        int y = mostrar.value(2).toInt(&ok);
        double m = Mapeo_x(x,y);
        double n = Mapeo_y(x,y);
        addPoint(m, n);
        qDebug() << x << y << mostrar.value(3).toInt(&ok) << mostrar.value(2).toInt(&ok);
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
        latitud = QString::number(serial->read(8).toInt(&ok));//.toDouble(&ok));
        longitud = QString::number(serial->read(8).toInt(&ok));//.toDouble(&ok));
        qDebug() << latitud << longitud;
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
}

void MainWindow::on_pushButtonmostrar_clicked()
{
    coordenadas(ui->lineEditcorner1->text(), ui->lineEditcorner2->text(), ui->lineEditcorner3->text(), ui->lineEditcorner4->text());
    mostrarDatos();
    plot();
}

void MainWindow::coordenadas(QString esq1, QString esq2, QString esq3, QString esq4)
{
    //----corners----
    //-31,747159, -60,515733
    //separación de coordenadas reales
    esq1.remove(QChar(','),Qt::CaseInsensitive);//.replace(",", ".");
    esq2.remove(QChar(','),Qt::CaseInsensitive);
    esq3.remove(QChar(','),Qt::CaseInsensitive);
    esq4.remove(QChar(','),Qt::CaseInsensitive);
    qDebug() << esq1 << esq2 << esq3 << esq4;
    X1 = esq1.right(8).toInt(&ok);
    esq1.chop(10);
    Y1 = esq1.right(8).toInt(&ok);
    int x2 = esq2.right(8).toInt(&ok);
    esq2.chop(10);
    int y2 = esq2.right(8).toInt(&ok);
    int x3 = esq3.right(8).toInt(&ok);
    esq3.chop(10);
    int y3 = esq3.right(8).toInt(&ok);
    int x4 = esq4.right(8).toInt(&ok);
    esq4.chop(10);
    int y4 = esq4.right(8).toInt(&ok);
    qDebug() << X1 << Y1 << x2 << y2 << x3 << y3 << x4 << y4;

    //mapeo de las esquinas
    //cálculo del ángulo entre rectas
    float coseno = (y2 - Y1) / (sqrt(pow((y2 - Y1),2) + pow((X1 - x2),2)));
    fi_rad = qAcos(coseno);
    if((X1 - x2) < 0)
        alfa_rad = -fi_rad;
    else
        alfa_rad = fi_rad;

    //coordenadas en el nuevo sistema
    //esquinas
    m1 = 0;
    n1 = 0;
    m2 = Mapeo_x(x2,y2);
    n2 = Mapeo_y(x2,y2);
    m3 = Mapeo_x(x3,y3);
    n3 = Mapeo_y(x3,y3);
    m4 = Mapeo_x(x4,y4);
    n4 = Mapeo_y(x4,y4);
    qDebug() << m1 << n1 << m2 << n2 << m3 << n3 << m4 << n4;

    //disposición de los límites de los ejes
    ui->plot->xAxis->setRange(m1,m4);
    ui->plot->yAxis->setRange(n1,n2);
    clearData();
    addPoint(m1,n1);
    addPoint(m2,n2);
    addPoint(m3,n3);
    addPoint(m4,n4);
    plot();
}

double MainWindow::Mapeo_x(double x, double y)
{
    xprima = (y - Y1)*sin(alfa_rad) - (X1 - x)*cos(alfa_rad);
    return xprima;
}

double MainWindow::Mapeo_y(double x, double y)
{
    yprima = (X1 - x + (y - Y1)*qSin(alfa_rad)*qCos(alfa_rad) - (X1 - x)*pow(cos(alfa_rad),2)) / qSin(alfa_rad);
    yprima = yprima - xprima * tan(0.085);
    return yprima;
}

void MainWindow::on_pushButton_clicked()
{
    QString punto_nuevo = ui->lineEditpunto->text();
    punto_nuevo.remove(QChar(','),Qt::CaseInsensitive);
    int x5 = punto_nuevo.right(8).toInt(&ok);
    punto_nuevo.chop(10);
    int y5 = punto_nuevo.right(8).toInt(&ok);
    m5 = Mapeo_x(x5,y5);
    n5 = Mapeo_y(x5,y5);
    addPoint(m5,n5);
    plot();
}
