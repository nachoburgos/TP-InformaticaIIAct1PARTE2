#ifndef VENTANAPRINCIPAL_H
#define VENTANAPRINCIPAL_H

#include <QDialog>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "ui_portcfg.h"
#include <QListWidgetItem>
#include <QString>
#include <QDebug>
#include <stdbool.h>
#include <cstdint>

#define header_datos 0x0B
#define header_resp 0x0C

QT_BEGIN_NAMESPACE
namespace Ui { class ventanaPrincipal; }
QT_END_NAMESPACE


typedef enum { stHeader = 0, ST_NivelAlto, ST_NivelBajo, stSuma } EstadoValues;

typedef union {
    unsigned short val;
    uint8_t p[2];
} shortVal;

typedef struct {
    shortVal value;
    uint8_t inx;
} shortParts;

typedef struct
{
    EstadoValues ActividadEstado;
    shortParts ValorAlto;
    shortParts ValorBajo;
    unsigned int sum;
} Estado;

typedef bool (*fcnState)(Estado *, unsigned char);

bool EstadoHeader(Estado *s, unsigned char newVal);
bool EstadoAlto(Estado *s, unsigned char newVal);
bool EstadoBajo(Estado *s, unsigned char newVal);
bool EstadoSumaComprobacion(Estado *s, unsigned char newVal);


class ventanaPrincipal : public QDialog
{
    Q_OBJECT

public:
    ventanaPrincipal(QWidget *parent = nullptr);
    ~ventanaPrincipal();

    QSerialPort *puertoSerie;
    void sendData(uint16_t t_on, uint16_t t_off);

private slots:
    void on_btnConectar_clicked();
    void on_btnDesconectar_clicked();
    void on_btnLimpiar_clicked();
    void on_tiempoEncendidoSlider_valueChanged(int value);
    void on_tiempoApagadoSlider_valueChanged(int value);
    void on_tiempoEncendidoSlider_sliderReleased();
    void on_tiempoApagadoSlider_sliderReleased();
    void on_lineEditTiempoEncendido_editingFinished();
    void on_lineEditTiempoApagado_editingFinished();
    void datos_recibidos();
    void sendData();

private:
    Ui::ventanaPrincipal *ui;
};

#endif // VENTANAPRINCIPAL_H
