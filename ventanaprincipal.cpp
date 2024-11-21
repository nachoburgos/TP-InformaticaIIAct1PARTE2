#include "ventanaprincipal.h"
#include "ui_ventanaprincipal.h"
#include "portcfg.h"
#include <QSerialPortInfo>

// Constructor
ventanaPrincipal::ventanaPrincipal(QWidget *parent) : QDialog(parent), ui(new Ui::ventanaPrincipal) {
    ui->setupUi(this);

    // Configuración del puerto serie
    puertoSerie = new QSerialPort();
    puertoSerie->setBaudRate(QSerialPort::Baud115200);
    puertoSerie->setDataBits(QSerialPort::Data8);
    puertoSerie->setFlowControl(QSerialPort::NoFlowControl);
    puertoSerie->setParity(QSerialPort::NoParity);
    puertoSerie->setStopBits(QSerialPort::TwoStop);

    // Configuración de la ventana y conexiones de señales
    this->setWindowTitle("Control (No Conectado)");
    connect(puertoSerie, SIGNAL(readyRead()), this, SLOT(datos_recibidos()));
    connect(ui->btnDeconectar, SIGNAL(clicked()), this, SLOT(on_btnDesconectar_clicked()));

    // Historial de comunicación serie
    ui->listWidget->addItem(new QListWidgetItem("HISTORIAL COMUNICACION SERIE:"));
}

// Destructor
ventanaPrincipal::~ventanaPrincipal() {
    // Desconectar antes de cerrar y eliminar el puerto serie
    disconnect(puertoSerie, SIGNAL(readyRead()), this, SLOT(datos_recibidos()));
    if (puertoSerie->isOpen())
        puertoSerie->close();
    delete puertoSerie;
    ui->listWidget->clear();
    delete ui;
}

// Función para enviar datos
void ventanaPrincipal::sendData() {
    if (puertoSerie->isOpen()) {
        // Obtener valores de sliders
        uint16_t t_on = ui->tiempoEncendidoSlider->value();
        uint16_t t_off = ui->tiempoApagadoSlider->value();

        // Construir arreglo de bytes a enviar
        uint8_t toSend[6];
        toSend[5] = header_datos +
                     (t_on % 256) +
                     (t_on >> 8) +
                     (t_off % 256) +
                     (t_off >> 8);

        toSend[0] = header_datos;
        toSend[1] = t_on % 256;
        toSend[2] = t_on >> 8;
        toSend[3] = t_off % 256;
        toSend[4] = t_off >> 8;

        // Mostrar valores enviados en hexadecimal
        QString v = " ";
        for (unsigned int k = 0; k < sizeof(toSend); k++) {
            v = v + QString::number((unsigned)toSend[k], 16) + "  ";
            puertoSerie->write((const char*)&toSend[k], 1);
        }
        qDebug() << v;

        // Mostrar mensaje en la interfaz
        ui->listWidget->addItem("ENVIO : : Configuracion a aplicar: T.E= " + QString::number(t_on) +
                                  " [ms] ; T.A= " + QString::number(t_off) + " [ms]");
    } else {
        // Mostrar error si no está conectado
        ui->listWidget->addItem("ERROR : : No conectado");
    }
}
// Funciones llamadas al editar las etiquetas
void ventanaPrincipal::on_lineEditTiempoEncendido_editingFinished() {
    // Validar y actualizar el valor del slider.
    int tiempoEncendido = ui->lineEditTiempoEncendido->text().toInt();

     if (tiempoEncendido > 2047) {
         ui->lineEditTiempoEncendido->setText(QString::number(2047));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     } else if (tiempoEncendido < 0) {
         ui->lineEditTiempoEncendido->setText(QString::number(0));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     }
     ui->tiempoEncendidoSlider->setValue(tiempoEncendido);
     // Enviar datos al editar la etiqueta
     sendData();
}


void ventanaPrincipal::on_lineEditTiempoApagado_editingFinished(){
    // Validar y actualizar el valor del slider.
    int tiempoApagado = ui->lineEditTiempoEncendido->text().toInt();

     if (tiempoApagado > 2047) {
         ui->lineEditTiempoApagado->setText(QString::number(2047));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     } else if (tiempoApagado < 0) {
         ui->labelTiempoApagado->setText(QString::number(0));
         ui->listWidget->addItem(new QListWidgetItem("ERROR!!! : : Rango de 0 a 2047 [ms]"));
     }
     ui->tiempoEncendidoSlider->setValue(tiempoApagado);
     // Enviar datos al editar la etiqueta
     sendData();
}

// Función llamada al hacer clic en el botón de conectar
void ventanaPrincipal::on_btnConectar_clicked() {
    // Crear ventana de configuración de puerto
    portCfg cfg(this);

    // Cerrar el puerto si ya está abierto
    if (puertoSerie->isOpen()) {
        puertoSerie->close();
    }

    // Obtener lista de puertos disponibles y agregar al ComboBox
    QList<QSerialPortInfo> puertos = QSerialPortInfo::availablePorts();
    foreach(QSerialPortInfo unPuerto, puertos ) {
        if (unPuerto.portName() != "")
            cfg.ui->cmbPuertos->addItem(unPuerto.portName());
    }

    // Abrir la ventana de configuración y conectar el puerto si se selecciona uno
    if(cfg.exec()) {
        puertoSerie->setPortName("\\\\.\\\\" + cfg.ui->cmbPuertos->currentText());
        puertoSerie->open(QIODevice::ReadWrite);
    }

    // Actualizar la interfaz según si se conecta o no
    if (puertoSerie->isOpen()) {
        this->setWindowTitle("Comunicacion Serie: (Conectado a: " + cfg.ui->cmbPuertos->currentText() + ")");
        ui->labEstado->setText("Conectado");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: Conectado puerto --> " + cfg.ui->cmbPuertos->currentText()));
        ui->btnDeconectar->setEnabled(true);
    } else {
        ui->labEstado->setText("Desconectado");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: No conectado"));
        this->setWindowTitle("Comserie (No Conectado)");
    }
}

// Función llamada al hacer clic en el botón de desconectar
void ventanaPrincipal::on_btnDesconectar_clicked() {
    // Desconectar el puerto y actualizar la interfaz


    if (puertoSerie->isOpen()) {
        puertoSerie->close();
        ui->labEstado->setText("Desconectado");
        this->setWindowTitle("Comserie (No Conectado)");
        ui->listWidget->addItem(new QListWidgetItem("CONN :: No conectado"));
    }
}

// Función llamada al hacer clic en el botón de limpiar historial
void ventanaPrincipal::on_btnLimpiar_clicked() {
    // Limpiar la lista de historial
    ui->listWidget->clear();
    ui->listWidget->addItem(new QListWidgetItem("HISTORIAL COMUNICACION SERIE:"));
}

// Función llamada al recibir datos en el puerto serie
void ventanaPrincipal::datos_recibidos() {
    // Máquina de estados para interpretar los datos recibidos
    static fcnState listEstado[4] = {
       EstadoHeader,
       EstadoAlto,
       EstadoBajo,
       EstadoSumaComprobacion
    };
    static Estado vEstado = {stHeader, {{0},0}, {{0},0}, 0};
    char newData;

    // Leer datos mientras estén disponibles
    while (puertoSerie->bytesAvailable() > 0) {
        puertoSerie->read(static_cast<char *>(&newData), 1);
        // Procesar datos según la máquina de estados
        if (listEstado[vEstado.ActividadEstado](&vEstado, newData)) {
            // Mostrar respuesta en la interfaz
            ui->listWidget->addItem("RESPUESTA : : Configuracion Aplicada: T.E= " + QString::number(vEstado.ValorAlto.value.val) +
                                      " [ms] ; T.A= " + QString::number(vEstado.ValorBajo.value.val) + " [ms]");
        }
    }
}

// Funciones para manejar cambios en sliders y etiquetas
void ventanaPrincipal::on_tiempoEncendidoSlider_valueChanged(int value) {
    Q_UNUSED(value);
    ui->lineEditTiempoEncendido->setText(QString::number(ui->tiempoEncendidoSlider->value()));
}

void ventanaPrincipal::on_tiempoApagadoSlider_valueChanged(int value) {
    Q_UNUSED(value);
    ui->lineEditTiempoApagado->setText(QString::number(ui->tiempoApagadoSlider->value()));
}

void ventanaPrincipal::on_tiempoEncendidoSlider_sliderReleased() {
    // Enviar datos al soltar el slider
    sendData();
}

void ventanaPrincipal::on_tiempoApagadoSlider_sliderReleased() {
    // Enviar datos al soltar el slider
    sendData();
}



// Máquina de estados
bool EstadoHeader(Estado *s, unsigned char newVal) {
    if(newVal == 0x0C) {
        s->ActividadEstado = ST_NivelAlto;
        s->ValorAlto.inx = 0;
        s->ValorBajo.inx = 0;
        s->sum = 0x0C;
    } else {
        s->ActividadEstado = stHeader;
    }
    return false;
}

bool EstadoAlto(Estado *s, unsigned char newVal) {
    s->ValorAlto.value.p[s->ValorAlto.inx] = newVal;
    s->sum += newVal;
    s->ValorAlto.inx++;
    if(s->ValorAlto.inx == sizeof(short))
        s->ActividadEstado = ST_NivelBajo;
    return false;
}

bool EstadoBajo(Estado *s, unsigned char newVal) {
    s->ValorBajo.value.p[s->ValorBajo.inx] = newVal;
    s->sum += newVal;
    s->ValorBajo.inx++;
    if(s->ValorBajo.inx == sizeof(short))
        s->ActividadEstado = stSuma;
    return false;
}

bool EstadoSumaComprobacion(Estado *s, unsigned char newVal) {
    bool ret = false;
    if(newVal == (s->sum % 256))
        ret = true;
    s->ActividadEstado = stHeader;
    return ret;
}
