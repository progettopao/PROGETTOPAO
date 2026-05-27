#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    // Inizializza l'applicazione Qt gestendo le risorse e il loop degli eventi
    QApplication a(argc, argv);

    // Crea l'istanza della finestra principale
    MainWindow w;

    // Mostra la finestra a schermo
    w.show();

    // Avvia il ciclo di esecuzione dell'applicazione
    return a.exec();
}
