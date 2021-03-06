#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "duthread.h"
#include "math.h"
#include <dbmanager.h>
#include <QQmlContext>

bool estado_serial = false, conf = false, pedido = false, bandera = false, dato_valido = false, ok;
double fi_rad, alfa_rad;
int m1, n1, m2, n2, m3, n3, m4, n4, m5, n5, m6, n6, m7, n7, m8, n8, xprima, yprima, X1, Y1;  //esquinas mapeadas

int cont_rep;
float vector2[col][fil];
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
    dialogoNewCancha = new DialogNewCourt();
    dialogoSelectCourt = new Dialog_SelectCourt;
    dialogoConexion = new Dialog_Conexion;
    dialogoGps = new Dialog_Gps;
    //----Serial----
    serial = new QSerialPort(this);
    connect(serial,SIGNAL(readyRead()),this,SLOT(Serial_Pedir()));
    //--------------
    connect(ui->actionserialConfig,SIGNAL(triggered(bool)),this,SLOT(on_action_Configurar_Conexi_n_triggered()));
    connect(ui->actionCancha,SIGNAL(triggered(bool)),this,SLOT(on_action_Seleccionar_Cancha_triggered()));
    connect(dialogoGps,SIGNAL(senal()),this,SLOT(buscarFecha()));
    connect(ui->actionJugador,SIGNAL(triggered(bool)),this,SLOT(on_actionNew_triggered()));
    connect(dialogoConexion,SIGNAL(senal()),this,SLOT(setCom()));
    //-----QML------
    ui->quickWidget->rootContext()->setContextProperty("mainWidget",this);
    ui->quickWidget->setSource(QUrl(QStringLiteral("qrc:/widgetPulsacion.qml")));
    this->showMaximized();

    deshabilitarConexion();

    openDatabase();
    initConfiguration();
    for(int i = 0; i < 11; i++){
        maxPulso[i] = 0;
        minPulso[i] = 300;
    }
}

MainWindow::~MainWindow()
{
    lastConfiguration();
    if(estado_serial)
        Serial_Desconect();
    delete ui;
}

void MainWindow::mostrarFechas(const QString &name)
{
    listaFechas.clear();
    QString consulta;
    consulta.append("SELECT fecha FROM ");
    consulta.append(name);
    QSqlQuery mostrar;
    mostrar.prepare(consulta);
    if(!mostrar.exec()){
        qDebug() << "ERROR! " << mostrar.lastError();
    }
    QDate fecha;
    while(mostrar.next()){
        fecha = mostrar.value(0).toDate();
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
            lastConfig.cancha = file->readLine();
            lastConfig.cancha.chop(1);
            lastConfig.puerto = file->readLine();
            lastConfig.puerto.chop(1);
        }
        readFile(lastConfig.cancha);
//        Serial_Conf(lastConfig.puerto);
        file->close();
    }
}

void MainWindow::lastConfiguration()
{
    file = new QFile("inicializador.txt");
    file->open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream flujo(file);
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
            QString titulo;
            titulo.append("GPSport - ");
            titulo.append(esquinas.nombreCancha);
            this->setWindowTitle(titulo);
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
        on_actionSerialConect_triggered();
    }
}

void MainWindow::Serial_Conect()
{
    if((serial->open(QIODevice::ReadWrite)))
    {
        estado_serial = true;
        serial->write("A");
        mThread->start(QThread::LowestPriority);
        for(int i=0;i<11;i++){
            maxPulso[i] = 0;
            minPulso[i] = 220;
            actualPulso[i] = 0;
            actualEstado[i] = false;
            ui->actionNew->setEnabled(false);
            ui->actionAdd_Court->setEnabled(false);
            ui->action_Seleccionar_Cancha->setEnabled(false);
            ui->action_Configurar_Conexi_n->setEnabled(false);
            ui->actionserialConfig->setEnabled(false);
            ui->actionJugador->setEnabled(false);
            ui->actionCancha->setEnabled(false);
        }
        ui->actionSerialConect->setIcon(QPixmap("://Icons/png/010-player.png"));
        emit serialConected();
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
    ui->actionSerialConect->setIcon(QPixmap("://Icons/png/032-play-arrow.png"));
    ui->actionNew->setEnabled(true);
    ui->actionAdd_Court->setEnabled(true);
    ui->action_Seleccionar_Cancha->setEnabled(true);
    ui->action_Configurar_Conexi_n->setEnabled(true);
    ui->actionserialConfig->setEnabled(true);
    ui->actionJugador->setEnabled(true);
    ui->actionCancha->setEnabled(true);
    emit serialDesconected();
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
    int n;
    DbManager::DataBlock data;
    if(serial->bytesAvailable() >= 13){
        data.validez = serial->read(1);
        if(data.validez < "q"){
            QString lat = "44";                 //acomodar los valores recibidos
            lat.append(serial->read(5));
            int lat_minutos = lat.toInt(&ok) / 6;
            lat.clear();
            lat = "31";
            lat.append(QString::number(lat_minutos));
            data.latitud = lat.toInt(&ok);

            QString lon = "30";
            lon.append(serial->read(5));
            int lon_minutos = lon.toInt(&ok) / 6;
            lon.clear();
            lon = "60";
            lon.append(QString::number(lon_minutos));
            data.longitud = lon.toInt(&ok);

            data.velocidad = serial->read(1).toHex().toInt(&ok,16);
            data.pulsacion = serial->read(1).toHex().toInt(&ok,16);
            data.hora = QTime::currentTime();
            data.fecha = QDate::currentDate();
            n = serialCamiseta(data.validez);
            if(data.pulsacion < minPulso[n])
                minPulso[n] = data.pulsacion;
            if(data.pulsacion > maxPulso[n])
                maxPulso[n] = data.pulsacion;
            actualPulso[n] = data.pulsacion;
            nombre = nombreCamiseta(n);
            db->addData(nombre,data);
            actualEstado[n] = true;
        }
        else
            serial->read(12);
//        qDebug() << data.validez << data.pulsacion;
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
                vector3[i-1][j-1] = A;
                vector3[i][j-1] = B;
                vector3[i+1][j-1] = C;
                vector3[i-1][j] = D;
                vector3[i][j] = E;
                vector3[i+1][j] = F;
                vector3[i-1][j+1] = G;
                vector3[i][j+1] = H;
                vector3[i+1][j+1] = I;
                //                vector3[i-1][j-1] = (A + E)*0.3;
                //                vector3[i][j-1] = (B + E)*0.75;
                //                vector3[i+1][j-1] = (C + E)*0.3;
                //                vector3[i-1][j] = (D + E)*0.75;
                //                vector3[i][j] = E;
                //                vector3[i+1][j] = (F + E)*0.75;
                //                vector3[i-1][j+1] = (G + E)*0.3;
                //                vector3[i][j+1] = (H + E)*0.75;
                //                vector3[i+1][j+1] = (I + E)*0.3;

//                vector3[i][j] = (A + B + C + D + E + F + G + H + I) / 9;

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
{                                       //CORRECCION DE ANGULO
    yprima = (X1 - x + (y - Y1)*qSin(alfa_rad)*qCos(alfa_rad) - (X1 - x)*pow(cos(alfa_rad),2)) / qSin(alfa_rad);
    yprima = yprima + xprima * tan(0.12);// - xprima * tan(0.02); //0.085);
//    if(yprima < 0)
//        yprima = 0;
    return yprima;
}

void MainWindow::on_actionNew_triggered()
{
    emit menuNuevoJugador();
}

void MainWindow::setNombre(const QString &name)
{
    nombre = quitarEspacio(name);
    buscarJugador(nombre);
}

void MainWindow::actualizarLista()
{
    listaJugadores = db->obtenerLista();
    foreach (const QString &str, listaJugadores){
        if((str != "perfiles")&&(str != "sqlite_sequence")){
            QString name = str;
            if(name.contains("_")){
                int u = name.indexOf("_");
                name.replace(u,1," ");
            }
        }
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

void MainWindow::mostrarAnalisis()
{
    mostrarFechas(nombre);
    dialogoGps->setListaFechas(listaFechas);
    coordenadas(esquinas.corner1, esquinas.corner2, esquinas.corner3, esquinas.corner4);
    dialogoGps->setModal(true);
    dialogoGps->show();
    for(int indice=0;indice<col;indice++)
        for(int jndice=0;jndice<fil;jndice++){
            vector3[indice][jndice] = 0;
        }
    dialogoGps->setNombre(agregarEspacio(nombre),perfilBuscado.maxppm);
    dialogoGps->plot(vector3,col);
    dialogoGps->clearData();
}

QString MainWindow::obtenerPulsacionMaxima(int numero)
{
    int pulso = maxPulso[numero];
//    if((pulso < 40)||(pulso > 255)){        //problema de maxima pulsacion
//        pulso = 0;
//        return "max";
//    }
    if(pulso == 0)
        return "max";
    else
        return QString::number(pulso);
}

QString MainWindow::obtenerPulsacionMinima(int numero)
{
    int pulso = minPulso[numero];
//    if((pulso < 40)||(pulso > 200)){
//        pulso = 0;
//        return "min";
//    }
    if(pulso == 300)
        return "min";
    else
        return QString::number(pulso);
}

QString MainWindow::obtenerPulsacionActual(int numero)
{
    int pulso = actualPulso[numero];
    if((pulso < 40)||(pulso > 220)){
        pulso = 0;
        return "---";
    }
    return QString::number(pulso);
}

bool MainWindow::obtenerEstado(int numero)
{
    bool estate = actualEstado[numero];
    actualEstado[numero] = false;
    return estate;
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
    QString titulo = "GPSport - ";
    titulo.append(esquinas.nombreCancha);
    this->setWindowTitle(titulo);
}

void MainWindow::on_action_Configurar_Conexi_n_triggered()
{
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
    if(estado_serial)
        Serial_Desconect();
    close();
}

bool MainWindow::on_actionSerialConect_triggered()
{
    if(!conf){
        on_action_Configurar_Conexi_n_triggered();
    }
    else{
        if(estado_serial){
            Serial_Desconect();
        }
        else{
            Serial_Conect();
        }
    }
    return estado_serial;
}

void MainWindow::buscarFecha()
{
    dialogoGps->clearData();
    for(int indice=0;indice<col;indice++)
        for(int jndice=0;jndice<fil;jndice++){
            vector3[indice][jndice] = 0;
            vector2[indice][jndice] = 0;
        }
    dialogoGps->plot(vector3,col);
    int contador = 0;
    int acumulador = 0;
    maxVelocidad = 0;
    pulsoMax = 0;
    pulsoMin = 220;
    int pulsoAct;
    float velAct;
    coordenadas(esquinas.corner1, esquinas.corner2, esquinas.corner3, esquinas.corner4);
    QDate fechaBuscada = dialogoGps->obtenerFecha();
    if(listaFechas.contains(fechaBuscada.toString("dd-MM-yyyy"))){
        max = 0;
        QString consulta;
        consulta.append("SELECT latitud,longitud,velocidad,pulsacion,hora,fecha FROM ");
        consulta.append(nombre);
        consulta.append(" WHERE fecha = (:fecha)");
        QSqlQuery mostrar;
        mostrar.prepare(consulta);
        mostrar.bindValue(":fecha", fechaBuscada.toString("yyyy-MM-dd"));
        if(!mostrar.exec()){
            qDebug() << "ERROR! " << mostrar.lastError();
        }
        while(mostrar.next()){
            pulsoAct = mostrar.value(3).toInt(&ok);
            velAct = mostrar.value(2).toInt(&ok) * 0.1;
            dialogoGps->setTiempoPulso(mostrar.value(4).toTime(),pulsoAct,velAct);
            if(velAct > maxVelocidad)
                maxVelocidad = velAct;
            if(mostrar.value(3).toInt(&ok) > pulsoMax)
                pulsoMax = mostrar.value(3).toInt(&ok);
            if(mostrar.value(3).toInt(&ok) < pulsoMin)
                pulsoMin = mostrar.value(3).toInt(&ok);
            acumulador += mostrar.value(3).toInt(&ok);
            contador++;
            int x = mostrar.value(1).toInt(&ok);
            int y = mostrar.value(0).toInt(&ok);
            int m = Mapeo_x(x,y);
            int n = Mapeo_y(x,y);
            int j = m / div;// - 4;        //CORRECCION DE CORRIMIENTO
            int k = n / div - 22;// - 32;   // + 18;//+ 20;
            if(j < 0 || j > col)
                j = 0;
            if(k < 0 || k > fil)
                k = 0;
            vector2[j][k] = vector2[j][k] + 1;
            if((vector2[j][k] > max))
                max = vector2[j][k];
        }
        pulsoProm = acumulador / contador;
//        if(maxVelocidad < 100)              //cálculo de velocidad
//            maxVelocidad = 0;
        maxVelocidad = maxVelocidad;
        dialogoGps->setDatos(maxVelocidad,pulsoMax,pulsoMin,pulsoProm);
        filtroMatricial();
        dialogoGps->plot(vector3,col);
        dialogoGps->setGrafPulsos();
    }
}

//----------------------

bool MainWindow::nuevoJugador(const QString &nombre, const QString &fecha, const QString &altura, const QString &peso, const QString &maxPPM)
{
    bool flag = false;
    if(!nombre.isEmpty() && !fecha.isEmpty() && !altura.isEmpty() && !peso.isEmpty()){
        QString nuevoNombre = quitarEspacio(nombre);
        perfilNuevo.nombre = nuevoNombre;
        perfilNuevo.fecha = formatoFecha(fecha);
        perfilNuevo.altura = altura.toInt(&ok);
        perfilNuevo.peso = peso.toInt(&ok);
        perfilNuevo.maxppm = maxPPM.toInt(&ok);
        bool flag1 = db->addPerfil(perfilNuevo);
        bool flag2 = db->createTable(nuevoNombre);
        flag = flag1&&flag2;
    }
    return flag;
}

bool MainWindow::buscarJugador(const QString &buscado)
{
    QString buscar = quitarEspacio(buscado);
    bool flagEncontrado = db->buscarPerfil(buscar);
    perfilBuscado = db->obtenerPerfil();
    perfilBuscado.nombre = agregarEspacio(perfilBuscado.nombre);
    return flagEncontrado;
}

QStringList MainWindow::obtenerJugador()
{
    QStringList perfilEncontrado;
    perfilEncontrado.append(perfilBuscado.nombre);
//    perfilEncontrado.append(perfilBuscado.fecha.toString("dd-MM-yyyy"));
    //---cálculo de edad---
    QDate hoy = QDate::currentDate();
    int edad = perfilBuscado.fecha.daysTo(hoy)/365;
    perfilEncontrado.append(QString::number(edad));
    //---------------------
    perfilEncontrado.append(QString::number(perfilBuscado.altura));
    perfilEncontrado.append(QString::number(perfilBuscado.peso));
    perfilEncontrado.append(QString::number(perfilBuscado.maxppm));
    perfilEncontrado.append(QString(perfilBuscado.photo));
    return perfilEncontrado;
}

QStringList MainWindow::obtenerListaJugadores()
{
    listaJugadores = db->obtenerLista();
    if(listaJugadores.contains("perfiles")){
        int x = listaJugadores.indexOf("perfiles");
        listaJugadores.removeAt(x);
    }
    if(listaJugadores.contains("sqlite_sequence")){
        int x = listaJugadores.indexOf("sqlite_sequence");
        listaJugadores.removeAt(x);
    }
    return listaJugadores;
}

void MainWindow::openDatabase()
{
    db = new DbManager("databaseTeam.sqlite");
    if(!db->isOpen()){
        qDebug() << "La database no pudo abrirse";
    }
    else {
        db->createTablePerfiles();
    }
}

QString MainWindow::quitarEspacio(QString nombre)
{
    if(nombre.contains(" ")){
        int u = nombre.indexOf(" ");
        nombre.replace(u,1,"_");
    }
    return nombre;
}

QString MainWindow::agregarEspacio(QString nombre)
{
    if(nombre.contains("_")){
        int u = nombre.indexOf("_");
        nombre.replace(u,1," ");
    }
    return nombre;
}

QDate MainWindow::formatoFecha(QString fecha)
{
    int dia, mes, ano;
    ano = fecha.right(4).toInt(&ok);
    fecha.chop(5);
    mes = fecha.right(2).toInt(&ok);
    fecha.chop(3);
    dia = fecha.toInt(&ok);
    QDate natalicio;
    natalicio.setDate(ano,mes,dia);
    return natalicio;
}

int MainWindow::serialCamiseta(const QString &player)
{
    int camiseta;
    if(player == "a")
        camiseta = 0;
    else if(player == "b")
        camiseta = 1;
    else if(player == "c")
        camiseta = 2;
    else if(player == "d")
        camiseta = 3;
    else if(player == "e")
        camiseta = 4;
    else if(player == "f")
        camiseta = 5;
    else if(player == "g")
        camiseta = 6;
    else if(player == "h")
        camiseta = 7;
    else if(player == "i")
        camiseta = 8;
    else if(player == "j")
        camiseta = 9;
    else if(player == "k")
        camiseta = 10;
    return camiseta;
}

QString MainWindow::nombreCamiseta(int camiseta)
{
    QString jugador;
    switch(camiseta){
        case 0:
            jugador = listaJugadores[0];
            break;
        case 1:
            jugador = listaJugadores[1];
            break;
        case 2:
            jugador = listaJugadores[2];
            break;
        case 3:
            jugador = listaJugadores[3];
            break;
        case 4:
            jugador = listaJugadores[4];
            break;
        case 5:
            jugador = listaJugadores[5];
            break;
        case 6:
            jugador = listaJugadores[6];
            break;
        case 7:
            jugador = listaJugadores[7];
            break;
        case 8:
            jugador = listaJugadores[8];
            break;
        case 9:
            jugador = listaJugadores[9];
            break;
        case 10:
            jugador = listaJugadores[10];
            break;
        default:
            break;
    }
    return jugador;
}

void MainWindow::calculoPrevioMaxMin()
{
    QString name, consulta;
    for(int i = 0; i<11; i++){
        name = nombreCamiseta(i);
        consulta.clear();
        consulta.append("SELECT pulsacion FROM ");
        consulta.append(name);
        QSqlQuery mostrar;
        mostrar.prepare(consulta);
        if(!mostrar.exec()){
            qDebug() << "ERROR! " << mostrar.lastError();
        }
        int pulso;
        maxPulso[i] = 0;
        minPulso[i] = 150;
        while(mostrar.next()){
            pulso = mostrar.value(0).toInt(&ok);
            if(pulso < minPulso[i])
                minPulso[i] = pulso;
            if(pulso > maxPulso[i])
                maxPulso[i] = pulso;
        }
    }
}

void MainWindow::habilitarConexion()
{
    ui->actionNew->setEnabled(true);
    ui->actionAdd_Court->setEnabled(true);
    ui->action_Seleccionar_Cancha->setEnabled(true);
    ui->action_Configurar_Conexi_n->setEnabled(true);
    ui->actionserialConfig->setEnabled(true);
    ui->actionJugador->setEnabled(true);
    ui->actionCancha->setEnabled(true);
    ui->actionSerialConect->setEnabled(true);
}

void MainWindow::deshabilitarConexion()
{
    ui->actionNew->setEnabled(false);
    ui->actionAdd_Court->setEnabled(false);
    ui->action_Seleccionar_Cancha->setEnabled(false);
    ui->action_Configurar_Conexi_n->setEnabled(false);
    ui->actionserialConfig->setEnabled(false);
    ui->actionJugador->setEnabled(false);
    ui->actionCancha->setEnabled(false);
    ui->actionSerialConect->setEnabled(false);
}

QString MainWindow::cargarPhoto_clicked()
{
    pathFoto = QFileDialog::getOpenFileName(
                this,
                "GPSport - Seleccionar una foto de perfil",
                "Users/javos/Documents/git/multiserie/",
                "Imágenes (*.jpg;*.jpeg;*.png);;All Files (*.*)");
    if(!pathFoto.isNull()){
        QFile file(pathFoto);
        file.open(QIODevice::ReadOnly);
        perfilNuevo.photo = file.readAll();
    }
    return QString(perfilNuevo.photo.toBase64());
}
