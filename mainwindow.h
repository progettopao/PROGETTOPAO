#ifndef MAINWINDOW_H
#define MAINWINDOW_H

// Classi Qt standard per la gestione della GUI
#include <QMainWindow>    // Finestra principale dell'applicazione
#include <QStackedWidget> // Contenitore di pagine intercambiabili a schermo
#include <QToolBar>       // Barra degli strumenti superiore
#include <QAction>        // Azioni per menu e toolbar
#include <QMenu>          // Menu a tendina
#include <QString>
#include <QVector>

// Classi di persistenza (Richieste dal tuo .cpp attuale)
#include "jsonpersistencemanager.h"
#include "xmlpersistencemanager.h"
#include "abstract_activity.h"

// Pre-dichiarazione delle classi grafiche separate (Moduli della View)
class ActivityListWidget;
class ActivityDetailWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT // Macro di Qt fondamentale per segnali e slot

private:
    // Componenti grafici della finestra principale
    QStackedWidget *centralStack;             // Gestisce lo switch dinamico delle schermate
    QToolBar *toolbar;                        // Barra degli strumenti in alto

    // STRUTTURE DATI E PERSISTENZA (Allineati al tuo .cpp)
    QVector<Abstract_Activity*> activityList; // Il vettore reale che contiene i puntatori polimorfi
    QString currentFilePath;                  // Traccia il percorso del file attualmente aperto
    JsonPersistenceManager jsonPersistence;   // Manager per il salvataggio/caricamento JSON
    XmlPersistenceManager xmlPersistence;     // Manager per il salvataggio/caricamento XML

    // Le pagine grafiche che verranno inserite dentro lo StackedWidget
    ActivityListWidget *listWidget;           // Elenco principale dei task con filtri e ricerca
    ActivityDetailWidget *detailWidget;       // Schermata di visualizzazione dei dettagli polimorfi

    // Metodi helper privati per l'inizializzazione della finestra
    void setupUI();                           // Alloca e posiziona i widget e i layout
    void setupMenus();                        // Configura la barra dei menu (File, Modifica, Aiuto)
    void setupConnections();                  // Centralizza i collegamenti connect() tra i widget
    void updateWindowTitle();                 // Aggiorna il titolo della finestra col file aperto
    void clearActivities();                   // Dealloca la memoria del vettore per evitare memory leak

public:
    MainWindow(QWidget *parent = nullptr);    // Costruttore standard
    ~MainWindow();                            // Distruttore

    // Restituisce il riferimento al vettore (Richiesto dal tuo .cpp e dai sotto-widget)
    QVector<Abstract_Activity*>& getActivityList();

public slots:
    // Slot di gestione del file (Barra dei menu)
    void newLibrary();                        // Reset dell'applicazione per un nuovo progetto vuoto
    void openLibrary();                       // Dialog di apertura file (Rileva in automatico JSON o XML)
    void saveLibrary();                       // Salvataggio rapido sul file corrente
    void saveLibraryAs();                     // Salvataggio con nome e scelta del formato (.json / .xml)

    // Slot di gestione delle attività (Interazione UI)
    void addActivity();                               // Apre la dialog interattiva per creare un nuovo task
    void editActivity(Abstract_Activity *activity);   // Apre la dialog precompilata per modificare il task
    void removeActivity(Abstract_Activity *activity); // Elimina un task chiedendo conferma

    // Slot di navigazione dello StackedWidget
    void showActivityDetails(Abstract_Activity *activity); // Mostra i dettagli del task selezionato
    void showActivityList();                               // Ritorna alla schermata dell'elenco principale

    // Slot di supporto globali
    void search();                            // Apre la dialog di ricerca e filtraggio
    void about();                             // Mostra la finestra popup con le informazioni del software
};

#endif // MAINWINDOW_H
