#include "dialog_gps.h"
#include "ui_dialog_gps.h"

#define cantidadMuestras 249
#define fcmax 200

QLinearGradient plotGradient;

Dialog_Gps::Dialog_Gps(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Gps)
{
    ui->setupUi(this);
    ui->plot->addGraph();
    colorScale = new QCPColorScale(ui->plot);
    initGrafPulsos();
    ui->plot->plotLayout()->addElement(0, 1, colorScale);
    ui->plot->setBackground(plotGradient);
    ui->plot->axisRect()->setBackground(Qt::white);
    ui->plot->xAxis->setVisible(false);
    ui->plot->yAxis->setVisible(false);
    ui->plot->yAxis2->setVisible(false);
    ui->plot->xAxis2->setVisible(false);
    ui->plot->setVisible(false);
    //-----pulsaciones-----
}

Dialog_Gps::~Dialog_Gps()
{
    delete ui;
}

void Dialog_Gps::on_pushButtonCerrar_clicked()
{
    this->close();
}

void Dialog_Gps::plot(float vector[][fil], int columnas)
{
    ui->plot->clearGraphs();
    ui->plot->axisRect()->setupFullAxesBox(true);
    QCPColorMap *colorMap = new QCPColorMap(ui->plot->xAxis, ui->plot->yAxis);
    int nx = columnas;
    int ny = fil;
    double x, y, z;
    colorMap->data()->setSize(nx, ny);
    colorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
    for (int xIndex = 0; xIndex < nx-1; xIndex++){
      for (int yIndex = 0; yIndex < ny-1; yIndex++){
        colorMap->data()->cellToCoord(xIndex, yIndex, &x, &y);
        z = vector[xIndex][yIndex];
        colorMap->data()->setCell(x, y, z);
        if(z > max)
            max = z;
      }
    }
    colorMap->setColorScale(colorScale);
    colorScale->axis()->setRange(QCPRange(0,max));
    colorScale->axis()->setVisible(false);
    QCPColorGradient miGradiente;
    miGradiente.clearColorStops();
    miGradiente.setColorStopAt(0.0,QColor("Blue"));
    miGradiente.setColorStopAt(0.3,QColor("Yellow"));
    miGradiente.setColorStopAt(0.5,QColor("Yellow"));
    miGradiente.setColorStopAt(1.0,QColor("Red"));
    colorMap->setGradient(miGradiente);
//  rescale the data dimension (color) such that all data points lie in the span visualized by the color gradient:
    colorMap->rescaleDataRange();
//  make sure the axis rect and color scale synchronize their bottom and top margins (so they line up):
    QCPMarginGroup *marginGroup = new QCPMarginGroup(ui->plot);
    ui->plot->axisRect()->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
    colorScale->setMarginGroup(QCP::msBottom|QCP::msTop, marginGroup);
//  rescale the key (x) and value (y) axes so the whole color map is visible:
    ui->plot->rescaleAxes();
    ui->plot->xAxis->setVisible(false);
    ui->plot->yAxis->setVisible(false);
    ui->plot->yAxis2->setVisible(false);
    ui->plot->xAxis2->setVisible(false);
    ui->plot->replot();
    ui->plot->update();

    ui->label_3->setScaledContents(true);
    ui->label_3->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    ui->label_4->setScaledContents(true);
    ui->label_4->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);

    QImage image = ui->plot->toPixmap().toImage();
    QImage image2 = QImage(image.width(),image.height(), QImage::Format_ARGB32);
    int r,g,b;
    for(int ix=0;ix<image.width();ix++)
        for(int iy=0;iy<image.height();iy++){
            QColor pixel = image.pixelColor(ix,iy);
            r = pixel.red();
            g = pixel.green();
            b = pixel.blue();

            if(pixel == QColor("Blue")){
                image2.setPixelColor(QPoint(ix,iy),QColor(r,g,b,0));
            }
            else if(b > 250 && r < 50 && g < 50){
                image2.setPixelColor(QPoint(ix,iy),QColor(r,g,b,127));
            }
            else{
                image2.setPixelColor(QPoint(ix,iy),QColor(r,g,b,255));
            }

//            if(pixel == QColor("Blue")){
//                image2.setPixelColor(QPoint(ix,iy),QColor(r,g,b,0));
//            }
//            else{
//                image2.setPixelColor(QPoint(ix,iy),QColor(r,g,b,255));
//            }
        }
    ui->label_3->setPixmap(QPixmap::fromImage(image2));
    QPixmap cancha;
    cancha.load(":/cancha_cesped_sin_lineas.jpg");
    ui->label_4->setPixmap(cancha);
}

void Dialog_Gps::setListaFechas(QStringList listaFechas)
{
    ui->comboBoxFecha->clear();
    ui->comboBoxFecha->insertItem(0,"Seleccione una fecha");
    foreach (const QString &str, listaFechas)
        ui->comboBoxFecha->insertItem(1,str);
}

QDate Dialog_Gps::obtenerFecha()
{
    return selectedFecha;
}

void Dialog_Gps::setGrafPulsos()
{
    initGrafPulsos();
    if(tiempo.size() != 0){
        int filas = tiempo.size();
        QVector<QCPGraphData> timeData(filas);
        QVector<double> tiempoFloat;
        double inicio = tiempo[0].hour() * 3600  + tiempo[0].minute() * 60 + tiempo[0].second() + tiempo[0].msec() * 1.0 / 1000;
        double final = tiempo[filas - 1].hour() * 3600 + tiempo[filas - 1].minute() * 60 + tiempo[filas - 1].second() + tiempo[filas - 1].msec() * 1.0 / 1000;
        for(int i = 0; i < filas; i++){
            tiempoFloat.append(tiempo[i].hour() * 3600 + tiempo[i].minute() * 60 + tiempo[i].second() + tiempo[i].msec() * 1.0 / 1000 - inicio);
        }
        ui->plotPulse->graph()->setData(tiempoFloat,pulsos);
        QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
        ui->plotPulse->xAxis->setTicker(timeTicker);
        timeTicker->setTimeFormat("%m:%s:%z");
        ui->plotPulse->xAxis->setRange(0, final - inicio);
        ui->plotPulse->yAxis->setRange(0, fcmax);
    }
    ui->plotPulse->replot();
}

void Dialog_Gps::setTiempoPulso(QTime time,int pulse)
{
    tiempo.append(time);
    pulsos.append(pulse);
}

void Dialog_Gps::clearData()
{
    ui->plotPulse->clearGraphs();
    tiempo.clear();
    pulsos.clear();
    ui->plotPulse->replot();
    setDatos(0,0,0,0);
}

void Dialog_Gps::initGrafPulsos()
{
    ui->plotPulse->addGraph();
    ui->plotPulse->setInteractions(QCP::iRangeDrag|QCP::iRangeZoom);
    ui->plotPulse->axisRect()->setRangeZoom(Qt::Horizontal);
    ui->plotPulse->axisRect()->setRangeDrag(Qt::Horizontal);
    ui->plotPulse->axisRect()->setupFullAxesBox(true);
    ui->plotPulse->xAxis->setBasePen(QPen(Qt::white, 1));
    ui->plotPulse->yAxis->setBasePen(QPen(Qt::white, 1));
    ui->plotPulse->xAxis->setTickPen(QPen(Qt::white, 1));
    ui->plotPulse->yAxis->setTickPen(QPen(Qt::white, 1));
    ui->plotPulse->xAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->plotPulse->yAxis->setSubTickPen(QPen(Qt::white, 1));
    ui->plotPulse->xAxis->setTickLabelColor(Qt::white);
    ui->plotPulse->yAxis->setTickLabelColor(Qt::white);
    ui->plotPulse->xAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plotPulse->yAxis->grid()->setPen(QPen(QColor(140, 140, 140), 1, Qt::DotLine));
    ui->plotPulse->xAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plotPulse->yAxis->grid()->setSubGridPen(QPen(QColor(80, 80, 80), 1, Qt::DotLine));
    ui->plotPulse->xAxis->grid()->setSubGridVisible(true);
    ui->plotPulse->yAxis->grid()->setSubGridVisible(true);
    ui->plotPulse->xAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->plotPulse->yAxis->grid()->setZeroLinePen(Qt::NoPen);
    ui->plotPulse->xAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    ui->plotPulse->yAxis->setUpperEnding(QCPLineEnding::esSpikeArrow);
    plotGradient.setStart(0, 0);
    plotGradient.setFinalStop(0, 350);
    plotGradient.setColorAt(0, QColor(80, 80, 80));
    plotGradient.setColorAt(1, QColor(50, 50, 50));
    ui->plotPulse->setBackground(plotGradient);
    QLinearGradient axisRectGradient;
    axisRectGradient.setStart(0, 0);
    axisRectGradient.setFinalStop(0, 350);
    axisRectGradient.setColorAt(0, QColor(80, 80, 80));
    axisRectGradient.setColorAt(1, QColor(30, 30, 30));
    ui->plotPulse->axisRect()->setBackground(axisRectGradient);
    ui->plotPulse->rescaleAxes();
    QSharedPointer<QCPAxisTickerText> textTicker(new QCPAxisTickerText);
    textTicker->addTick(fcmax,"100");
    textTicker->addTick(fcmax*0.9, "90");
    textTicker->addTick(fcmax*0.8, "80");
    textTicker->addTick(fcmax*0.7, "70");
    textTicker->addTick(fcmax*0.6, "60");
    textTicker->addTick(fcmax*0.5, "50");
    textTicker->addTick(fcmax*0.4, "40");
    textTicker->addTick(fcmax*0.3, "30");
    ui->plotPulse->yAxis->setTicker(textTicker);
    ui->plotPulse->xAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->plotPulse->yAxis->setTickLabelFont(QFont(QFont().family(), 8));
    ui->plotPulse->xAxis->setLabel("Tiempo de entrenamiento [m:s:ms]");
    ui->plotPulse->yAxis->setLabel("% fcmax");
    ui->plotPulse->xAxis->setLabelFont(QFont(QFont().family(), 14));
    ui->plotPulse->yAxis->setLabelFont(QFont(QFont().family(), 14));
    ui->plotPulse->xAxis->setLabelColor(QColor("White"));
    ui->plotPulse->yAxis->setLabelColor(QColor("White"));
    ui->plotPulse->xAxis2->setVisible(true);
    ui->plotPulse->yAxis2->setVisible(true);
    ui->plotPulse->xAxis2->setTicks(false);
    ui->plotPulse->yAxis2->setTicks(false);
    ui->plotPulse->xAxis2->setTickLabels(false);
    ui->plotPulse->yAxis2->setTickLabels(false);
    ui->plotPulse->graph(0)->setName("ppm en entrenamiento");
    ui->plotPulse->legend->setVisible(true);
    ui->plotPulse->legend->setBrush(QColor(255, 255, 255, 150)); 
    ui->plotPulse->replot();
}

void Dialog_Gps::setNombre(const QString &name)
{
    this->setWindowTitle(name);
}

void Dialog_Gps::setDatos(const float &maxVel, const int &maxPul, const int &minPul, const int &promPul)
{
    ui->labelMaxPulsaciones->setText(QString::number(maxPul).append(" ppm"));
    ui->labelMinPulsaciones->setText(QString::number(minPul).append(" ppm"));
    ui->labelPromPulsaciones->setText(QString::number(promPul).append(" ppm"));
    ui->labelVelocidad->setText(QString::number(maxVel,'f',2).append(" m/s"));
}

void Dialog_Gps::on_comboBoxFecha_activated(const QString &arg1)
{
    selectedFecha = QDate::fromString(arg1,"dd-MM-yyyy");
    tiempo.clear();
    pulsos.clear();
    emit senal();
}
