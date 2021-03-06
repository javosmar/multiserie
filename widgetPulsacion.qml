import QtQuick 2.6
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import QtCharts 2.2

Item {
    id: mainParent
    width: 1024
    height: 730

    property real anchoBoton: (mainParent.width - 20) / 3
    property int altoBoton: 50
    property int cantidadJugadores: 11

    Image {
        fillMode: Image.PreserveAspectCrop
        anchors.fill: parent
        source: "qrc:/Background/munich-allianz-arena-120289.jpeg"
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: inicio
    }

    Connections {
        target: mainWidget
        onMenuNuevoJugador: {
            stack.push(crearJugador)
        }
    }

    Component {
        id: inicio

        Item {

            Image {
                id: backInicio
                fillMode: Image.PreserveAspectCrop
                width: mainParent.width
                height: mainParent.height
                source: "qrc:/Background/Soccer-Field-Night.jpg"
            }

            Rectangle {
                id: backCat
                width: backCatFrame.width + 70
                height: backCatFrame.height + 70
                anchors.horizontalCenter: backInicio.horizontalCenter
                y: (backInicio.height - backCat.height) / 2 - 60
                color: '#B5151515'

                Frame {
                    id: backCatFrame
                    width: titulo.paintedWidth + 70
                    height: titulo.paintedHeight + subtitulo.paintedHeight + 70
                    anchors.horizontalCenter: backCat.horizontalCenter
                    anchors.verticalCenter: backCat.verticalCenter
                }

                Text {
                    id: titulo
                    text: "Nombre"
                    y: (backCat.height - (titulo.height + subtitulo.height)) / 2
                    anchors.horizontalCenter: backCat.horizontalCenter
                    font.pointSize: 75
                    font.bold: true
                    color: 'steelblue'
                }

                Text {
                    id: subtitulo
                    font.pointSize: 17
                    text: "Tracker GPS para deportes de equipo"
                    anchors.top: titulo.bottom
                    anchors.horizontalCenter: backCat.horizontalCenter
                    color: 'white'
                }
            }

            Button {
                id: buttonAtras
                width: anchoBoton
                height: altoBoton
                x: mainParent.width - buttonAtras.width - 5
                y: mainParent.height - buttonAtras.height - 5
                text: 'AVANZAR'
                font.pointSize: 15
                onClicked: {
                    mainWidget.habilitarConexion();
                    stack.push(pulsacion);
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: buttonAnimIn
                    running: buttonAtras.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonAtras; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: profileAnimOut
                    running: !buttonAtras.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonAtras; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }
            }
        }
    }

    Component {
        id: pulsacion

        Item {
            property bool visibilidad: false

            Component.onCompleted: {
                grilla.model.clear();
                var listaJugadores = mainWidget.obtenerListaJugadores();
                if(listaJugadores.length !== 0){
                var maxPulso
                var minPulso
                for(var ix=0;ix<listaJugadores.length;ix++){
                    var Jugador = listaJugadores[ix];
                    var flag = mainWidget.buscarJugador(Jugador);
                    if(flag){
                        var jugador = mainWidget.obtenerJugador(Jugador);
                        maxPulso = mainWidget.obtenerPulsacionMaxima(ix);
                        minPulso = mainWidget.obtenerPulsacionMinima(ix);
                        grilla.model.append({
                                                Jugador: jugador[0],
                                                Foto: "data:image/png;base64," + jugador[5],
                                                Fecha: "Edad: " + jugador[1],
                                                Peso: "Peso: " + jugador[2],
                                                Altura: "Altura: " + jugador[3],
                                                MaxPPM: "Fcmax: " + jugador[4],
                                                Pulso: "---",
                                                Max: maxPulso,
                                                Min: minPulso,
                                                visibilidad: false});
                    }
                }
                }
            }

            GridView {
                id: grilla
                width: mainParent.width - 5
                height: mainParent.height - 5
                x: 5
                y: 5
                cellWidth: grilla.width / 6
                cellHeight: (grilla.height - buttonInicio.height) / 2
                model: ListModel { }
                delegate: Rectangle {
                    id: myParent
                    width: grilla.cellWidth - 5
                    height: grilla.cellHeight - 10
                    color: '#5FF300F0'


                    Text {
                        x: 7
                        y: 5
                        id: textNombre
                        font.pointSize: 20
                        color: 'white'
                        text: Jugador
                    }

                    Image {
                        id: pic
                        anchors.left: textNombre.left
                        anchors.top: textNombre.bottom
                        anchors.topMargin: 5
                        width: grilla.cellWidth * 0.4;
                        height: grilla.cellHeight * 0.4;
                        fillMode: Image.PreserveAspectCrop
                        source: Foto
                        Frame{
                            anchors.fill: pic
                        }
                    }

                    Text {
                        id: textFecha
                        font.pointSize: 15
                        anchors.left: textNombre.left
                        anchors.top: pic.bottom
                        anchors.topMargin: 10
                        color: 'white'
                        text: Fecha + " años"
                    }

                    Text {
                        id: textAltura
                        font.pointSize: 15
                        anchors.left: textNombre.left
                        anchors.top: textFecha.bottom
                        anchors.topMargin: 10
                        color: 'white'
                        text: Altura + " cm"
                    }

                    Text {
                        id: textPeso
                        font.pointSize: 15
                        anchors.left: textNombre.left
                        anchors.top: textAltura.bottom
                        anchors.topMargin: 10
                        color: 'white'
                        text: Peso + " kg"
                    }

                    Text {
                        id: textPPM
                        font.pointSize: 15
                        anchors.left: textNombre.left
                        anchors.top: textPeso.bottom
                        anchors.topMargin: 10
                        color: 'white'
                        text: MaxPPM + " ppm"
                    }

                    MouseArea {
                        id: perfil
                        anchors.fill: myParent
                        enabled: !buttonConectar.estadoConexion

                        onClicked: {
                            mainWidget.setNombre(Jugador);
                            mainWidget.mostrarAnalisis();
                        }
                        hoverEnabled: true;
                        property bool señalado: false
                        onEntered: {
                            señalado = true;
                        }
                        onExited: {
                            señalado = false;
                        }

                        SequentialAnimation{
                            id: profileAnimIn
                            running: perfil.señalado
                            alwaysRunToEnd: true

                            PropertyAnimation { target: myParent; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                        }
                        SequentialAnimation{
                            id: profileAnimOut
                            running: !perfil.señalado
                            alwaysRunToEnd: true

                            PropertyAnimation { target: myParent; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                        }
                    }

                    Column {
                        id: columnaEstadistica
                        anchors {
                            right: myParent.right
                            verticalCenter: pic.verticalCenter
                            rightMargin: (myParent.width - pic.width - columnaEstadistica.width - 5) / 2
                            top: pic.top
                            topMargin: 10
                        }
                        spacing: 5

                        Text {
                            id: textPulso
                            anchors.horizontalCenter: columnaEstadistica.horizontalCenter
                            font.pointSize: 30
                            font.bold: true
                            color: 'white'
                            text: Pulso
                        }

                        Text {
                            id: textMaxPulsometro
                            anchors.horizontalCenter: columnaEstadistica.horizontalCenter
                            font.pointSize: 15
                            color: 'white'
                            text: Min + " - " + Max
                        }
                    }

                    Image {
                        id: heart
                        anchors.horizontalCenter: columnaEstadistica.horizontalCenter
//                        anchors.top: columnaEstadistica.bottom
                        anchors.bottom: pic.bottom
//                        y: columnaEstadistica.y + pic.height / 2//columnaEstadistica.height/2
                        width: ((myParent.width - pic.width) * 0.4)
                        height: width
                        source: "qrc:/Icons/heart.png"
                        smooth: true
                        antialiasing: true
                        visible: visibilidad && buttonConectar.estadoConexion

                        SequentialAnimation{
                            id: heartAnim
                            running: buttonConectar.estadoConexion
                            loops: Animation.Infinite
                            alwaysRunToEnd: true

                            PropertyAnimation { target: heart; property: "scale"; to: 1.2; duration: 500; easing.type: Easing.InQuad }
                            PropertyAnimation { target: heart; property: "scale"; to: 1.0; duration: 500; easing.type: Easing.OutQuad }
                        }

                        Connections {
                            target: mainWidget
                            onValidador: {
//                              codigo para detectar unidades online y offline
                            }
                        }
                    }
                }
            }


            Timer {
                id: timer
                interval: 1000
                running: buttonConectar.estadoConexion
                repeat: true

                property bool estadoOnline: false

                onTriggered: {
                    var listaJugadores = mainWidget.obtenerListaJugadores();
                    var maxPulso
                    var minPulso
                    var actualPulso
                    var estado
                    for(var ix=0;ix<listaJugadores.length;ix++){
                        var Jugador = listaJugadores[ix];
                        maxPulso = mainWidget.obtenerPulsacionMaxima(ix);
                        minPulso = mainWidget.obtenerPulsacionMinima(ix);
                        actualPulso = mainWidget.obtenerPulsacionActual(ix);
                        estado = mainWidget.obtenerEstado(ix);
                        grilla.model.get(ix).Pulso = actualPulso
                        grilla.model.get(ix).Max = maxPulso
                        grilla.model.get(ix).Min = minPulso
                        grilla.model.get(ix).visibilidad = estado
                    }
                }
            }

            Button {
                id: buttonNuevoJugador
                width: anchoBoton
                height: altoBoton
                x: mainParent.width - buttonInicio.width - 5
                y: mainParent.height - buttonInicio.height - 5
                text: 'NUEVO JUGADOR'
                font.pointSize: 15
                enabled: !buttonConectar.estadoConexion

                onClicked: {
                    mainWidget.deshabilitarConexion();
                    stack.push(crearJugador);
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: buttonAnimIn
                    running: buttonNuevoJugador.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonNuevoJugador; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: buttonAnimOut
                    running: !buttonNuevoJugador.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonNuevoJugador; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }
            }

            Button {
                id: buttonConectar
                width: anchoBoton
                height: altoBoton
                x: buttonNuevoJugador.width + 10
                y: mainParent.height - buttonConectar.height - 5
                font.pointSize: 15
                text: textBoton
                checkable: true
                checked: estadoConexion
                onClicked: {
                    mainWidget.on_actionSerialConect_triggered();
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: button2AnimIn
                    running: buttonConectar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonConectar; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: button2AnimOut
                    running: !buttonConectar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonConectar; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }

                property bool estadoConexion: false
                property string textBoton: "COMENZAR A ENTRENAR"

                Connections {
                    target: mainWidget
                    onSerialConected: {
                        buttonConectar.estadoConexion = true
                        buttonConectar.textBoton = "FIN DEL ENTRENAMIENTO"
                    }
                    onSerialDesconected: {
                        buttonConectar.estadoConexion = false
                        buttonConectar.textBoton = "COMENZAR A ENTRENAR"
                    }
                }

                BusyIndicator {
                    id: busyIndication
                    width: altoBoton
                    height: altoBoton
                    anchors.right: buttonConectar.right
                    running: buttonConectar.estadoConexion
                 }
            }

            Button {
                id: buttonInicio
                width: anchoBoton
                height: altoBoton
                x: 5
                y: mainParent.height - buttonNuevoJugador.height - 5
                text: 'VOLVER'
                font.pointSize: 15
                enabled: !buttonConectar.estadoConexion

                onClicked: {
                    mainWidget.deshabilitarConexion();
                    stack.push(inicio);
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: button3AnimIn
                    running: buttonInicio.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonInicio; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: button3AnimOut
                    running: !buttonInicio.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonInicio; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }
            }
        }
    }

    Component {
        id: crearJugador
        Item {

            Rectangle {
                id: backIngresoDatos
                width: mainParent.width / 3
                height: mainParent.height / 2
                x: mainParent.width / 2 - backIngresoDatos.width / 2
                y: mainParent.height / 2 - backIngresoDatos.height / 2
                color: '#B5151515'

                Grid {
                    id: grillaNuevoJugador
                    anchors.horizontalCenter: backIngresoDatos.horizontalCenter
                    anchors.top: backIngresoDatos.top
                    anchors.topMargin: 15
                    columns: 2
                    columnSpacing: 5
                    spacing: 5

                    Label {
                        id: nombreLabel
                        text: 'Nombre y apellido'
                        color: 'white'
                        font.pointSize: 15
                    }

                    TextField {
                        id: nombreTextField
                        KeyNavigation.tab: fechaTextField
                        font.pointSize: 15
                        width: nombreLabel.width
                        height: nombreLabel.paintedHeight + 10
                    }

                    Label {
                        id: fechaLabel
                        text: 'Fecha de nacimiento'
                        color: 'white'
                        font.pointSize: 15
                    }

                    TextField {
                        id: fechaTextField
                        KeyNavigation.tab: alturaTextField
                        font.pointSize: 15
                        width: fechaLabel.width * 0.75
                        height: fechaLabel.paintedHeight + 10
                    }

                    Label {
                        id: alturaLabel
                        text: 'Altura [cm]'
                        color: 'white'
                        font.pointSize: 15
                    }

                    TextField {
                        id: alturaTextField
                        KeyNavigation.tab: pesoTextField
                        font.pointSize: 15
                        width: alturaLabel.width * 0.75
                        height: alturaLabel.paintedHeight + 10
                    }

                    Label {
                        id: pesoLabel
                        text: 'Peso [kg]'
                        color: 'white'
                        font.pointSize: 15
                    }

                    TextField {
                        id: pesoTextField
                        KeyNavigation.tab: maxPulsoTextField
                        font.pointSize: 15
                        width: alturaTextField.width
                        height: pesoLabel.paintedHeight + 10
                    }
                    Label {
                        id: maxPulsoLabel
                        text: 'fcmax [ppm]'
                        color: 'white'
                        font.pointSize: 15
                    }

                    TextField {
                        id: maxPulsoTextField
                        KeyNavigation.tab: nombreTextField
                        font.pointSize: 15
                        width: alturaTextField.width
                        height: maxPulsoLabel.paintedHeight + 10
                    }
                }

                Column {
                    spacing: 5
                    anchors.horizontalCenter: backIngresoDatos.horizontalCenter
                    anchors.top: grillaNuevoJugador.bottom
                    anchors.topMargin: 15

                    Image {
                        id: picNuevoPerfil
                        width: backIngresoDatos.width / 4
                        height: backIngresoDatos.height / 3
                        fillMode: Image.PreserveAspectFit
                        source: source1
                        property string source1: "qrc:/noPhoto.png"

                        MouseArea {
                            id: perfilFoto
                            anchors.fill: picNuevoPerfil
                            property string source2: "data:image/png;base64,"
                            onClicked: {
                                picNuevoPerfil.source = source2 + mainWidget.cargarPhoto_clicked();
                            }

                            hoverEnabled: true;
                            property bool señalado: false
                            onEntered: {
                                señalado = true;
                            }
                            onExited: {
                                señalado = false;
                            }

                            SequentialAnimation{
                                id: profileAnimIn
                                running: perfilFoto.señalado
                                alwaysRunToEnd: true

                                PropertyAnimation { target: picNuevoPerfil; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                            }
                            SequentialAnimation{
                                id: profileAnimOut
                                running: !perfilFoto.señalado
                                alwaysRunToEnd: true

                                PropertyAnimation { target: picNuevoPerfil; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                            }
                        }
                    }
                }
            }

            Button {
                id: buttonGuardar
                width: anchoBoton
                height: altoBoton
                x: mainParent.width - buttonGuardar.width - 5
                y: mainParent.height - buttonGuardar.height - 5
                text: 'GUARDAR'
                font.pointSize: 15

                onClicked: {
                    var flag = mainWidget.nuevoJugador(nombreTextField.text,fechaTextField.text,
                                            alturaTextField.text,pesoTextField.text,maxPulsoTextField.text);
                    if(flag){
                        stack.push(pulsacion);
                        mainWidget.habilitarConexion();
                    }
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: buttonAnimIn
                    running: buttonGuardar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonGuardar; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: buttonAnimOut
                    running: !buttonGuardar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonGuardar; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }
            }

            Button {
                id: buttonCancelar
                width: anchoBoton
                height: altoBoton
                x: 5
                y: mainParent.height - buttonCancelar.height - 5
                text: 'CANCELAR'
                font.pointSize: 15

                onClicked: {
                    mainWidget.habilitarConexion();
                    stack.push(pulsacion);
                }

                hoverEnabled: true
                SequentialAnimation{
                    id: button2AnimIn
                    running: buttonCancelar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonCancelar; property: "scale"; to: 1.0; duration: 150; easing.type: Easing.InQuad }
                }
                SequentialAnimation{
                    id: button2AnimOut
                    running: !buttonCancelar.hovered
                    alwaysRunToEnd: true

                    PropertyAnimation { target: buttonCancelar; property: "scale"; to: 0.95; duration: 150; easing.type: Easing.OutQuad }
                }
            }
        }
    }
}
