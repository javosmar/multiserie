#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"
#include "math.h"
#include <dbmanager.h>

#define col 110
#define fil 64
#define div 10

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
double fi_rad, alfa_rad;
int m1, n1, m2, n2, m3, n3, m4, n4, m5, n5, m6, n6, m7, n7, m8, n8, xprima, yprima, X1, Y1;  //esquinas mapeadas

int cont_rep;
int vector[1500][1500], vector2[col][fil];
float vector3[col][fil];
double max = 0;

QString nombrejugador;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("GPSport");
    mThread = new DuThread(10,this);
    connect(mThread,&DuThread::valorCambiado, ui->progressBarserie, &QProgressBar::setValue);
    dialogoNew = new Dialog_nuevo;
    dialogoHR = new Dialog_HRate;
    dialogoNewCancha = new DialogNewCourt();
    dialogoSelectCourt = new Dialog_SelectCourt;
    dialogoConexion = new Dialog_Conexion;
    dialogoGps = new Dialog_Gps;
    ui->actionJugador->setEnabled(false);
    ui->actionNew->setEnabled(false);
    //----Serial----
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Pedir()));
    connect(dialogoHR,SIGNAL(senal()),this,SLOT(Serial_Desconect()));
    connect(dialogoHR,SIGNAL(finished(int)),this,SLOT(Serial_Desconect()));
    //--------------
    connect(ui->actionserialConfig,SIGNAL(triggered(bool)),this,SLOT(on_action_Configurar_Conexi_n_triggered()));
    connect(ui->actionCancha,SIGNAL(triggered(bool)),this,SLOT(on_action_Seleccionar_Cancha_triggered()));
    connect(ui->actionAnalisis,SIGNAL(triggered(bool)),this,SLOT(on_actionMostrar_Analisis_triggered()));
    connect(ui->actionJugador,SIGNAL(triggered(bool)),this,SLOT(on_actionNew_triggered()));
    connect(ui->actionEquipo,SIGNAL(triggered(bool)),this,SLOT(on_action_Open_triggered()));

    initConfiguration();
}

MainWindow::~MainWindow()
{
    lastConfiguration();
    delete ui;
}

void MainWindow::mostrarTabla()
{
    QString consulta;
    consulta.append("SELECT * FROM ");
    consulta.append(nombre);
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(!mostrar.exec()){
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
        ui->tableWidgetdato->setItem(fila,5,new QTableWidgetItem(mostrar.value(6).toDate().toString("dd-MM-yyyy")));
        fila++;
    }
}

void MainWindow::mostrarDatos()
{
    for(int indice=0;indice<1500;indice++)
        for(int jndice=0;jndice<1500;jndice++)
            vector[indice][jndice] = 0;
    for(int indice=0;indice<col;indice++)
        for(int jndice=0;jndice<fil;jndice++)
            vector2[indice][jndice] = 0;
    cont_rep = 0;
    QString consulta;
    consulta.append("SELECT * FROM ");
    consulta.append(nombre);
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(!mostrar.exec()){
        qDebug() << "ERROR! " << mostrar.lastError();
    }
    max = 0;
    QDate fecha;
    ui->tableWidgetdato->setRowCount(0);
    while(mostrar.next()){
        int x = mostrar.value(3).toInt(&ok);
        int y = mostrar.value(2).toInt(&ok);
        int m = Mapeo_x(x,y);
        int n = Mapeo_y(x,y);
        int j = m / div;
        int k = n / div;
        vector[m][n] = vector[m][n] + 1;
        vector2[j][k] = vector2[j][k] + 1;
        if((vector2[j][k] > max))
            max = vector2[j][k];
    }
}

void MainWindow::mostrarFechas()
{
    listaFechas.clear();
    QString consulta;
    consulta.append("SELECT * FROM ");
    consulta.append(nombre);
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(!mostrar.exec()){
        qDebug() << "ERROR! " << mostrar.lastError();
    }
    QDate fecha;
    while(mostrar.next()){
        fecha = mostrar.value(6).toDate();
        QString date = fecha.toString("dd-MM-yyyy");
        if(!listaFechas.contains(date))
            listaFechas.insert(0,date);
    }
}

void MainWindow::initConfiguration()
{
    file = new QFile("inicializador.txt");
    if(file->open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream flujo(file);
        while(!flujo.atEnd()){
            lastConfig.database = file->readLine();
            lastConfig.database.chop(1);
            lastConfig.jugador = file->readLine();
            lastConfig.jugador.chop(1);
            lastConfig.cancha = file->readLine();
            lastConfig.cancha.chop(1);
            lastConfig.puerto = file->readLine();
            lastConfig.puerto.chop(1);
        }
        path = lastConfig.database;
        ejecutarNuevo();
        on_comboBoxtablas_activated(lastConfig.jugador);
        ui->comboBoxtablas->setCurrentText(lastConfig.jugador);
        readFile(lastConfig.cancha);
        ui->labelCanchaSelected->setText(esquinas.nombreCancha);
        Serial_Conf(lastConfig.puerto);
        file->close();
    }
}

void MainWindow::lastConfiguration()
{
    file = new QFile("inicializador.txt");
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flujo(file);
    flujo << path << "\n";
    flujo << lastConfig.jugador << "\n";
    flujo << lastConfig.cancha << "\n";
    flujo << lastConfig.puerto << "\n";
    file->close();
}

void MainWindow::openFile()
{
    if (!file->open(QIODevice::ReadWrite | QIODevice::Text))
        return;
}

void MainWindow::writeFile(QString texto)
{
    file = new QFile("canchas.txt");
    openFile();
    QTextStream flujo(file);
    flujo.seek(file->size());
    flujo << texto << "\n";
    file->close();
    dialogoNewCancha->limpiarCancha();
}

void MainWindow::readFile(QString palabra)
{
    file = new QFile("canchas.txt");
    openFile();
    QTextStream flujo(file);
    QString buscado;
    int posicion = 0;
    bool encontrado = false;
    while(!encontrado){
        buscado = flujo.readLine();
        if (buscado.contains(palabra, Qt::CaseInsensitive)){
            esquinas.corner4 = buscado.right(22);
            buscado.chop(23);
            esquinas.corner3 = buscado.right(22);
            buscado.chop(23);
            esquinas.corner2 = buscado.right(22);
            buscado.chop(23);
            esquinas.corner1 = buscado.right(22);
            buscado.chop(23);
            esquinas.nombreCancha = buscado;
            encontrado = true;
        }
        else
            posicion++;
    }
    file->close();
}

void MainWindow::Serial_Conf(QString puertoNombre)
{
    serial->setPortName(puertoNombre);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);
    lastConfig.puerto = puertoNombre;
    if(!conf){
        conf = true;
//        on_actionSerialConect_triggered();
    }
}

void MainWindow::Serial_Conect()
{
    if((serial->open(QIODevice::ReadWrite)))
    {
        qDebug() << "puerto abierto";
        estado_serial = true;
        serial->write("A");
        mThread->start(QThread::LowestPriority);
        dialogoHR->show();
        ui->actionSerialConect->setIcon(QPixmap("://Icons/png/010-player.png"));
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
    mThread->terminate();
//    mostrarDatos();
    ui->actionSerialConect->setIcon(QPixmap("://Icons/png/032-play-arrow.png"));
}

void MainWindow::Serial_Error()
{
    QMessageBox error;
    error.setWindowTitle("GPSport - Error");
    error.setText("Verifique la conexión con la unidad central.");
    error.setIcon(QMessageBox::Warning);
    error.exec();
}

void MainWindow::Serial_Pedir()
{
    DbManager::DataBlock data;
    if(serial->bytesAvailable() >= 25){
        data.validez = serial->read(1);
        QString lat = serial->read(2);
        int lat_minutos = serial->read(6).toInt(&ok);// / 6;
        lat.append(QString::number(lat_minutos));
        data.latitud = lat.toInt(&ok);
        serial->read(2);
        QString lon = serial->read(2);
        int lon_minutos = serial->read(6).toInt(&ok);// / 6;
        lon.append(QString::number(lon_minutos));
        data.longitud = lon.toInt(&ok);
        serial->read(1);
        data.velocidad = serial->read(4).toInt(&ok);
        data.pulsacion = serial->read(1);
        data.fecha.setDate(2010,11,19);
//        data.fecha = QDate::currentDate();
        db->addData(nombre,data);
        qDebug() << data.validez << data.latitud << data.longitud << data.velocidad << data.pulsacion << data.fecha;
        dialogoHR->setHrPlayer(data.validez, data.velocidad);
    }
}

/*
 *  A   B   C
 *  D   E   F
 *  G   H   I
 *
 *  A   B   C
 *  D   E   F
 *  G   H   I
 */

void MainWindow::filtroMatricial()
{
    float A,B,C,D,E,F,G,H,I;
    int xlim = col - 1;
    int ylim = fil - 1;
    for(int i=1;i<xlim;i++)
        for(int j=1;j<ylim;j++){
            if(vector2[i][j] > 0){
                A = vector2[i-1][j-1];
                B = vector2[i][j-1];
                C = vector2[i+1][j-1];
                D = vector2[i-1][j];
                E = vector2[i][j];
                F = vector2[i+1][j];
                G = vector2[i-1][j+1];
                H = vector2[i][j+1];
                I = vector2[i+1][j+1];
                vector3[i-1][j-1] = (A + E)*0.3;
                vector3[i][j-1] = (B + E)*0.75;
                vector3[i+1][j-1] = (C + E)*0.3;
                vector3[i-1][j] = (D + E)*0.75;
                vector3[i][j] = E;
                vector3[i+1][j] = (F + E)*0.75;
                vector3[i-1][j+1] = (G + E)*0.3;
                vector3[i][j+1] = (H + E)*0.75;
                vector3[i+1][j+1] = (I + E)*0.3;
            }
        }
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

void MainWindow::on_pushButtonmanual_clicked()
{
    QString punto_nuevo = ui->lineEditpunto->text();
    punto_nuevo.remove(QChar(','),Qt::CaseInsensitive);
    int x5 = punto_nuevo.right(8).toInt(&ok);
    punto_nuevo.chop(10);
    int y5 = punto_nuevo.right(8).toInt(&ok);
    m5 = Mapeo_x(x5,y5);
    n5 = Mapeo_y(x5,y5);
    DbManager::DataBlock currentData;
    currentData.validez = "C";
    currentData.latitud = y5;
    currentData.longitud = x5;
    currentData.velocidad = 125;
    currentData.pulsacion = "@";
    db->addData(nombre,currentData);
}

void MainWindow::on_pushButtonrandom_clicked()
{
    int aleatorioy = (qrand() % 622) + 747308;
    int aleatoriox = (qrand() % 1098) + 514665;
    QString punto;
    punto.append("-31,");
    punto.append(QString::number(aleatorioy));
    punto.append(", -60,");
    punto.append(QString::number(aleatoriox));
    ui->lineEditpunto->setText(punto);
}

void MainWindow::on_action_Open_triggered()
{
    path.clear();
    path = QFileDialog::getSaveFileName(
                this,
                "GPSport - Abrir o crear equipo",
                "/Users/javos/Desktop/",
                "Databases (*.sqlite);;All Files (*.*)");
    if(!path.isNull())
        ejecutarNuevo();
}

void MainWindow::ejecutarNuevo()
{
    db = new DbManager(path);
    if(db->isOpen()){
        QString titulo;
        titulo.append("GPSport - ");
        titulo.append(path);
        this->setWindowTitle(titulo);
        ui->comboBoxtablas->setEnabled(true);
        ui->actionNew->setEnabled(true);
        ui->actionJugador->setEnabled(true);
        actualizarLista();
    }
}

void MainWindow::on_actionNew_triggered()
{
    connect(dialogoNew,SIGNAL(senal()),this,SLOT(ejecutarNuevoJugador()));
    dialogoNew->setModal(true);
    dialogoNew->adjustSize();
    dialogoNew->setWindowTitle("Nuevo Jugador");
    dialogoNew->show();
}

void MainWindow::ejecutarNuevoJugador()
{
    disconnect(dialogoNew,0,this,0);
    Dialog_nuevo::perfil p = dialogoNew->obtenerPerfil();
    QString cadena;
    cadena = p.nombre;
    if(cadena.contains(" ")){
        int u = cadena.indexOf(" ");
        cadena.replace(u,1,"_");
    }
    if(db->createTable(cadena)){
        ui->actionSerialConect->setEnabled(true);
        actualizarLista();
    }
    if(db->createTablePerfiles()){
        DbManager::PerfilBlock currentPerfil;
        currentPerfil.nombre = cadena;
        currentPerfil.photo = p.photo;
        currentPerfil.fecha = p.fecha;
        currentPerfil.peso = p.peso;
        currentPerfil.altura = p.altura;
        db->addPerfil(currentPerfil);
    }
}

void MainWindow::actualizarLista()
{
    ui->comboBoxtablas->clear();
    QStringList lista;
    lista = db->obtenerLista();
    ui->comboBoxtablas->insertItem(0,"Seleccione un jugador");
    foreach (const QString &str, lista){
        if((str != "perfiles")&&(str != "sqlite_sequence")){
            QString name = str;
            if(name.contains("_")){
                int u = name.indexOf("_");
                name.replace(u,1," ");
            }
            ui->comboBoxtablas->insertItem(1,name);
        }
    }
}

void MainWindow::on_comboBoxtablas_activated(const QString &arg1)
{
    nombre = arg1;
    if(arg1 != lastConfig.jugador)
        lastConfig.jugador = arg1;
    if(nombre.contains(" ")){
        int u = nombre.indexOf(" ");
        nombre.replace(u,1,"_");
    }
    if(db->buscarPerfil(nombre)){
        DbManager::PerfilBlock q = db->obtenerPerfil();
        QPixmap foto;
        foto.loadFromData(q.photo);
        ui->labelBusqueda->setPixmap(foto);
    }
    else{
        ui->labelBusqueda->setPixmap(QPixmap(":/noPhoto.png"));
    }
}

void MainWindow::on_actionAdd_Court_triggered()
{
    connect(dialogoNewCancha,SIGNAL(senal()),this,SLOT(agregarCancha()));
    dialogoNewCancha->setModal(true);
    dialogoNewCancha->show();
}

void MainWindow::agregarCancha()
{
    disconnect(dialogoNewCancha,0,this,0);
    QString cancha = dialogoNewCancha->obtenerCancha();
    writeFile(cancha);
}

void MainWindow::on_actionMostrar_Analisis_triggered()
{
    mostrarFechas();
    dialogoGps->setListaFechas(listaFechas);
    coordenadas(esquinas.corner1, esquinas.corner2, esquinas.corner3, esquinas.corner4);
    mostrarDatos();
    filtroMatricial();
    dialogoGps->setModal(true);
    dialogoGps->plot(vector3,110);
    dialogoGps->show();
    for(int indice=0;indice<col;indice++)
        for(int jndice=0;jndice<fil;jndice++)
            vector3[indice][jndice] = 0;
    connect(dialogoGps,SIGNAL(senal()),this,SLOT(buscarFecha()));
}

void MainWindow::on_action_Seleccionar_Cancha_triggered()
{
    connect(dialogoSelectCourt,SIGNAL(senal()),this,SLOT(seleccionCancha()));
    dialogoSelectCourt->actualizarCanchas();
    dialogoSelectCourt->setModal(true);
    dialogoSelectCourt->show();
}

void MainWindow::seleccionCancha()
{
    disconnect(dialogoSelectCourt,0,this,0);
    QString court;
    court = dialogoSelectCourt->obtenerCancha();
    lastConfig.cancha = court;
    readFile(court);
    ui->labelCanchaSelected->setText(esquinas.nombreCancha);
}

void MainWindow::on_action_Configurar_Conexi_n_triggered()
{
    connect(dialogoConexion,SIGNAL(senal()),this,SLOT(setCom()));
    dialogoConexion->setModal(true);
    dialogoConexion->show();
}

void MainWindow::setCom()
{
    disconnect(dialogoConexion,0,this,0);
    QString com = dialogoConexion->obtenerPuerto();
    Serial_Conf(com);
}

void MainWindow::on_actionClose_triggered()
{
    lastConfiguration();
    close();
}

void MainWindow::on_actionSerialConect_triggered()
{
    if(!conf){
        on_action_Configurar_Conexi_n_triggered();
    }
    else{
        if(estado_serial){
            Serial_Desconect();
//            mostrarDatos();
        }
        else{
            Serial_Conect();
        }
    }
}

void MainWindow::buscarFecha()
{
    coordenadas(esquinas.corner1, esquinas.corner2, esquinas.corner3, esquinas.corner4);
    QDate fechaBuscada = dialogoGps->obtenerFecha();
    if(listaFechas.contains(fechaBuscada.toString("dd-MM-yyyy")))
    {
        for(int indice=0;indice<col;indice++)
            for(int jndice=0;jndice<fil;jndice++){
                vector2[indice][jndice] = 0;
                vector3[indice][jndice] = 0;
            }
        max = 0;
        QString consulta;
        consulta.append("SELECT latitud,longitud,velocidad,pulsacion,fecha FROM ");
        consulta.append(nombre);
        consulta.append(" WHERE fecha = (:fecha)");
        QSqlQuery mostrar;
        mostrar.prepare(consulta);
        mostrar.bindValue(":fecha", fechaBuscada);
        if(!mostrar.exec()){
            qDebug() << "ERROR! " << mostrar.lastError();
        }
        while(mostrar.next()){
            int x = mostrar.value(1).toInt(&ok);
            int y = mostrar.value(0).toInt(&ok);
            int m = Mapeo_x(x,y);
            int n = Mapeo_y(x,y);
            int j = m / div;
            int k = n / div;
//            vector[m][n] = vector[m][n] + 1;
            vector2[j][k] = vector2[j][k] + 1;
            if((vector2[j][k] > max))
                max = vector2[j][k];
        }
        filtroMatricial();
        dialogoGps->plot(vector3,fil);
    }
}

void MainWindow::on_pushButton_clicked()
{
    mostrarTabla();
    mostrarFechas();
}
