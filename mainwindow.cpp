#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"
#include "math.h"

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
QString validez, latitud, longitud, velocidad, pulsacion;
double fi_rad, alfa_rad;
int m1, n1, m2, n2, m3, n3, m4, n4, m5, n5, m6, n6, m7, n7, m8, n8, xprima, yprima, X1, Y1;  //esquinas mapeadas
QImage fondo;

int cont_rep;
int vector[1500][1500], vector2[110][64];
QCPColorScale *colorScale;
double max = 0;

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
    colorScale = new QCPColorScale(ui->plot);
    ui->plot->plotLayout()->addElement(0, 1, colorScale);
//    colorScale->axis()->setRange(QCPRange(0,max));
//    ui->plot->graph(0)->setScatterStyle(QCPScatterStyle::ssCross);//::ssCircle);
//    ui->plot->graph(0)->setLineStyle(QCPGraph::lsNone);
//    fondo.load("://soccer field.png");
//    ui->plot->setBackground(fondo);
//    ui->plot->xAxis->setVisible(false);
//    ui->plot->yAxis->setVisible(false);
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
//    mostrarDatos();
    //--------------
    //club Paraná
//    ui->lineEditcorner1->setText("-31,747308, -60,514664");
//    ui->lineEditcorner2->setText("-31,747931, -60,514819");
//    ui->lineEditcorner3->setText("-31,747757, -60,515908");
//    ui->lineEditcorner4->setText("-31,747135, -60,515763");
    //casa
    ui->lineEditcorner1->setText("-31,748118, -60,515155");
    ui->lineEditcorner2->setText("-31,748958, -60,515475");
    ui->lineEditcorner3->setText("-31,748782, -60,516285");
    ui->lineEditcorner4->setText("-31,747961, -60,516121");
    //club Patronato
//    ui->lineEditcorner1->setText("-31,742903, -60,506960");
//    ui->lineEditcorner2->setText("-31,743539, -60,507244");
//    ui->lineEditcorner3->setText("-31,743193, -60,508320");
//    ui->lineEditcorner4->setText("-31,742563, -60,508043");
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
//        qDebug() << "usuario agregado correctamente";
    }
    else{
//        qDebug() << "ERROR! " << insertar.lastError();
    }
}

void MainWindow::mostrarDatos()
{
    for(int indice=0;indice<1500;indice++)
        for(int jndice=0;jndice<1500;jndice++)
            vector[indice][jndice] = 0;
    for(int indice=0;indice<110;indice++)
        for(int jndice=0;jndice<64;jndice++)
            vector2[indice][jndice] = 0;
    cont_rep = 0;
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
    max = 0;
    ui->tableWidgetdato->setRowCount(0);
    while(mostrar.next()){
        ui->tableWidgetdato->insertRow(fila);
        ui->tableWidgetdato->setItem(fila,0,new QTableWidgetItem(mostrar.value(1).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,1,new QTableWidgetItem(mostrar.value(2).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,2,new QTableWidgetItem(mostrar.value(3).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,3,new QTableWidgetItem(mostrar.value(4).toByteArray().constData()));
        ui->tableWidgetdato->setItem(fila,4,new QTableWidgetItem(mostrar.value(5).toByteArray().constData()));
        if(mostrar.value(1) == "b")
            cont_rep++;
        int x = mostrar.value(3).toInt(&ok);
        int y = mostrar.value(2).toInt(&ok);
        int m = Mapeo_x(x,y);
        int n = Mapeo_y(x,y);
        addPoint(m, n);
        int j = m / 10;
        int k = n / 10;
        vector[m][n] = vector[m][n] + 1;
        vector2[j][k] = vector2[j][k] + 1;
//        if((vector2[j][k] > max))
//            max = vector2[j][k];
        if((vector2[j][k] > max) && (ui->verticalSlider_max->value() == 1)){
            max = vector2[j][k];
            colorScale->axis()->setRange(QCPRange(0,max));
        }
        else{
            max = ui->verticalSlider_max->value();
            colorScale->axis()->setRange(QCPRange(0,ui->verticalSlider_max->value()));
        }
        if(vector2[j][k] > max)
            vector2[j][k] = max;
        ui->label_max->setText(QString::number(max));
        ui->verticalSlider_max->setSliderPosition(max);
//        qDebug() << j << k << vector2[j][k];
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
        latitud = serial->read(2);
        int lat_minutos = serial->read(7).toInt(&ok) / 6;
        latitud.append(QString::number(lat_minutos));
        int basura = serial->read(1).toInt(&ok);
        longitud = serial->read(2);
        int lon_minutos = serial->read(7).toInt(&ok) / 6;
        longitud.append(QString::number(lon_minutos));
        velocidad = serial->read(4);
        pulsacion = serial->read(1);
        insertarUsuario();
//        qDebug() << validez << latitud << longitud << velocidad << pulsacion;
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
//    mostrarDatos();
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
    // configure axis rect:
    //ui->plot->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom); // this will also allow rescaling the color scale by dragging/zooming
    ui->plot->axisRect()->setupFullAxesBox(true);
    // set up the QCPColorMap:
    QCPColorMap *colorMap = new QCPColorMap(ui->plot->xAxis, ui->plot->yAxis);
//    int nx = m4;
//    int ny = n2;

    int nx = 110;
    int ny = 64;
    double x, y, z;
//    max = 0;
    QImage *img = new QImage("://Captura.PNG");
    if(img->isNull())
        qDebug() << "Error al cargar el archivo de imagen";
    else
    {
        int xindice = 0;
        int yindice = 0;
        int contx = 0;
        int conty = 0;
        colorMap->data()->setSize(771, 577); // we want the color map to have nx * ny data points
        colorMap->data()->setRange(QCPRange(0, 771), QCPRange(0, 577));//(m1, m4), QCPRange(n1, n2)); // and span the coordinate range -4..4 in both key (x) and value (y) dimensions now we assign some data, by accessing the QCPColorMapData instance of the color map:

        for (int xIndex = 0; xIndex < 770; xIndex++){

          for (int yIndex = 0; yIndex < 576; yIndex++){
            colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
            double gris = ((double)qGray(img->pixel(xIndex,yIndex)));
            if((gris > 250))// && (max > 0))
                gris = max;
            z = gris + vector2[xindice][yindice];
//            qDebug() << xindice << yindice << contx << conty << xIndex << yIndex << vector2[xindice][yindice];
//            if(vector2[xindice][yindice] > max)
//                max = vector2[xindice][yindice];
            colorMap->data()->setCell(x, y, z);
            if(conty == 8){
                conty = 0;
                yindice++;
            }
            else
                conty++;
          }
          yindice = 0;
          if(contx == 6){
              contx = 0;
              xindice++;
              yindice = 0;
          }
          else
              contx++;
        }
    }
    qDebug() << max;
//    for (int xIndex = 0; xIndex < nx; ++xIndex){
//      for (int yIndex = 0; yIndex < ny; ++yIndex){
//        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
//        z = vector2[xIndex][yIndex];
//        if(vector2[xIndex][yIndex] > max)
//            max = vector2[xIndex][yIndex];
//        colorMap->data()->setCell(xIndex, yIndex, z);
//      }
//    }
    // add a color scale:
//    QCPColorScale *colorScale = new QCPColorScale(ui->plot);
//    ui->plot->plotLayout()->addElement(0, 1, colorScale); // add it to the right of the main axis rect
//    colorScale->setType(QCPAxis::atRight); // scale shall be vertical bar with tick/axis labels right (actually atRight is already the default)
    colorMap->setColorScale(colorScale); // associate the color map with the color scale
    //    colorScale->axis()->setRange(QCPRange(0,max));
    //colorScale->axis()->setLabel("Magnetic Field Strength");

    // set the color gradient of the color map to one of the presets:
    colorMap->setGradient(QCPColorGradient::gpGrayscale); // we could have also created a QCPColorGradient instance and added own colors to the gradient, see the documentation of QCPColorGradient for what's possible.

    //    colorMap->setInterpolate(false);
    // rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();

    // make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->plot);
    ui->plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);

    // rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->plot->rescaleAxes();



    //ui->plot->graph(0)->setData(qv_x, qv_y);
    ui->plot->replot();
    ui->plot->update();
}

void MainWindow::on_pushButtonmostrar_clicked()
{
    coordenadas(ui->lineEditcorner1->text(), ui->lineEditcorner2->text(), ui->lineEditcorner3->text(), ui->lineEditcorner4->text());
    mostrarDatos();
    //plot();
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

    //disposición de los límites de los ejes
//    ui->plot->xAxis->setRange(m1,m4);
//    ui->plot->yAxis->setRange(n1,n2);
    clearData();
    addPoint(m1,n1);
    addPoint(m2,n2);
    addPoint(m3,n3);
    addPoint(m4,n4);
    //plot();
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
    validez = "C";
    latitud = QString::number(y5);
    longitud = QString::number(x5);
    velocidad = "0.321";
    pulsacion = "@";
    insertarUsuario();
//    mostrarDatos();
//    plot();
//    qDebug() << x5 << y5 << m5 << n5;
}

void MainWindow::contador()
{

}

void MainWindow::on_pushButton_2_clicked()
{
//    -31,747308, -60,514664
//    -31,747931, -60,514819
//    -31,747757, -60,515908
//    -31,747135, -60,515763
    int aleatorioy = (qrand() % 622) + 747308;
    int aleatoriox = (qrand() % 1098) + 514665;
    QString punto;
    punto.append("-31,");
    punto.append(QString::number(aleatorioy));
    punto.append(", -60,");
    punto.append(QString::number(aleatoriox));
    ui->lineEditpunto->setText(punto);
}

void MainWindow::on_verticalSlider_max_actionTriggered(int action)
{
    ui->label_max->setText(QString::number(ui->verticalSlider_max->value()));
}
