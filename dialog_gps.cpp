#include "dialog_gps.h"
#include "ui_dialog_gps.h"

Dialog_Gps::Dialog_Gps(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog_Gps)
{
    ui->setupUi(this);

    ui->plot->addGraph();
    colorScale = new QCPColorScale(ui->plot);
    ui->plot->plotLayout()->addElement(0, 1, colorScale);
    ui->plot->setBackground(Qt::lightGray);
    ui->plot->axisRect()->setBackground(Qt::white);
    ui->plot->xAxis->setVisible(false);
    ui->plot->yAxis->setVisible(false);
    ui->plot->yAxis2->setVisible(false);
    ui->plot->xAxis2->setVisible(false);
}

Dialog_Gps::~Dialog_Gps()
{
    delete ui;
}

void Dialog_Gps::on_pushButtonCerrar_clicked()
{
    this->close();
}

void Dialog_Gps::plot(float vector[][fil], int filas)
{
//    ui->plot->graph(0)->data().clear();
    ui->plot->clearGraphs();
    ui->plot->axisRect()->setupFullAxesBox(true);
    QCPColorMap *colorMap = new QCPColorMap(ui->plot->xAxis, ui->plot->yAxis);
    int nx = filas;
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
    miGradiente.setColorStopAt(0.0,QColor("Cyan"));
    miGradiente.setColorStopAt(0.1,QColor("Blue"));
    miGradiente.setColorStopAt(0.6,QColor("Yellow"));
    miGradiente.setColorStopAt(0.9,QColor("Orange"));
    miGradiente.setColorStopAt(1.0,QColor("Dark red"));
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
    ui->plot->replot();
    ui->plot->update();
}

void Dialog_Gps::setListaFechas(QStringList listaFechas)
{
    ui->comboBoxFecha->clear();
    ui->comboBoxFecha->insertItem(0,"Seleccione la fecha del entrenamiento");
    foreach (const QString &str, listaFechas)
        ui->comboBoxFecha->insertItem(1,str);
}

QDate Dialog_Gps::obtenerFecha()
{
    return selectedFecha;
}

void Dialog_Gps::on_comboBoxFecha_activated(const QString &arg1)
{
    bool ok;
    int dia, mes, ano;
    QString date = arg1;
    ano = date.right(4).toInt(&ok);
    date.chop(5);
    mes = date.right(2).toInt(&ok);
    date.chop(3);
    dia = date.toInt(&ok);
    selectedFecha.setDate(ano,mes,dia);
    emit senal();
}
