#include "activitylistwidget.h"
#include "mainwindow.h" // Serve per poter invocare mainWindow->getActivityList()

// Includiamo le classi concrete per i controlli di tipo con dynamic_cast
#include "bill.h"
#include "hometask.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

#include <QHeaderView>
#include <QMessageBox>

ActivityListWidget::ActivityListWidget(MainWindow *mainWin)
    : QWidget(mainWin), mainWindow(mainWin) {
    
    setupUI();
    setupConnections();
    
    // Al momento della creazione, popola la tabella con i dati correnti
    updateActivityList();
}

void ActivityListWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // --- AREA FILTRI (Barra Superiore) ---
    QHBoxLayout *filterLayout = new QHBoxLayout();
    
    filterLayout->addWidget(new QLabel("Visualizzazione:", this));
    viewModeCombo = new QComboBox(this);
    viewModeCombo->addItems({"Tutte le attività", "Solo Urgenti"});
    filterLayout->addWidget(viewModeCombo);
    
    filterLayout->addWidget(new QLabel("Categoria:", this));
    filterTypeCombo = new QComboBox(this);
    filterTypeCombo->addItems({"Tutte le categorie", "Bill", "HomeTask", "VehicleMaintenance", "ShoppingTask", "LeisureTimeTask"});
    filterLayout->addWidget(filterTypeCombo);
    
    filterLayout->addStretch(); // Spinge i filtri a sinistra
    
    addButton = new QPushButton("Nuova Attività", this);
    filterLayout->addWidget(addButton);
    
    mainLayout->addLayout(filterLayout);

    // --- TABELLA DATI (Area Centrale) ---
    tableWidget = new QTableWidget(this);
    setupTableHeaders();
    
    // Ottimizzazioni grafiche per la tabella
    tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // Seleziona la riga intera
    tableWidget->setSelectionMode(QAbstractItemView::SingleSelection); // Una sola riga selezionabile
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);   // Celle non modificabili direttamente con un click
    tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch); // Colonne auto-ridimensionabili
    
    mainLayout->addWidget(tableWidget);
}

void ActivityListWidget::setupTableHeaders() {
    QStringList headers;
    headers << "ID" << "Titolo" << "Categoria" << "Stato" << "Urgenza";
    tableWidget->setColumnCount(headers.size());
    tableWidget->setHorizontalHeaderLabels(headers);
}

void ActivityListWidget::setupConnections() {
    // Connessione dei filtri (scattano quando l'utente cambia selezione nei ComboBox)
    connect(viewModeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ActivityListWidget::changeViewMode);
    connect(filterTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ActivityListWidget::filterByType);
    
    // Pulsante rapido per aggiungere attività (rimanda alla MainWindow)
    connect(addButton, &QPushButton::clicked, mainWindow, &MainWindow::addActivity);
    
    // Gestione del click singolo (selezione) e del doppio click (apertura dettagli) sulla tabella
    connect(tableWidget, &QTableWidget::itemSelectionChanged, this, &ActivityListWidget::handleSelection);
    connect(tableWidget, &QTableWidget::cellDoubleClicked, this, &ActivityListWidget::handleCellDoubleClick);
}

// IL CUORE DELLA VIEW: Prende i dati dal Modello e li trasforma in elementi grafici
void ActivityListWidget::updateActivityList() {
    tableWidget->setRowCount(0); // Svuota completamente la tabella per ridisegnarla
    
    // Recupera il riferimento costante al vettore di attività memorizzato nella MainWindow (Vincolo 7)
    const QVector<Abstract_Activity*>& activities = mainWindow->getActivityList();
    
    QString filtroVisualizzazione = viewModeCombo->currentText();
    QString filtroCategoria = filterTypeCombo->currentText();
    
    int row = 0;
    for (Abstract_Activity* activity : activities) {
        if (!activity) continue;
        
        // 1. Controllo Filtro Urgenza (Polimorfismo puro - Vincolo 8)
        if (filtroVisualizzazione == "Solo Urgenti" && !activity->isUrgente()) {
            continue; 
        }
        
        // 2. Controllo Filtro Categoria tramite RTTI / dynamic_cast (Vincolo 9 - Niente stringhe nel flusso)
        QString stringaCategoria = "Generica";
        if (dynamic_cast<Bill*>(activity)) {
            stringaCategoria = "Bill";
        } else if (dynamic_cast<HomeTask*>(activity)) {
            stringaCategoria = "HomeTask";
        } else if (dynamic_cast<VehicleMaintenance*>(activity)) {
            stringaCategoria = "VehicleMaintenance";
        } else if (dynamic_cast<ShoppingTask*>(activity)) {
            stringaCategoria = "ShoppingTask";
        } else if (dynamic_cast<LeisureTimeTask*>(activity)) {
            stringaCategoria = "LeisureTimeTask";
        }
        
        // Se l'utente ha impostato una categoria specifica e questa non coincide con quella dell'oggetto, salta
        if (filtroCategoria != "Tutte le categorie" && filtroCategoria != stringaCategoria) {
            continue;
        }
        
        // 3. Inserimento grafico dei dati nella riga della tabella
        tableWidget->insertRow(row);
        
        // Cella ID
        tableWidget->setItem(row, 0, new QTableWidgetItem(activity->getId()));
        
        // Cella Titolo
        tableWidget->setItem(row, 1, new QTableWidgetItem(activity->getTitolo()));
        
        // Cella Categoria (calcolata sopra)
        tableWidget->setItem(row, 2, new QTableWidgetItem(stringaCategoria));
        
        // Cella Stato (Completata o In Corso)
        QString statoText = activity->isCompletata() ? "✅ Completata" : "⏳ In Corso";
        tableWidget->setItem(row, 3, new QTableWidgetItem(statoText));
        
        // Cella Urgenza (Polimorfica)
        QString urgenzaText = activity->isUrgente() ? "🔥 URGENTE" : "Normale";
        tableWidget->setItem(row, 4, new QTableWidgetItem(urgenzaText));
        
        row++;
    }
}

// Slot attivato quando l'utente clicca su una riga della tabella
void ActivityListWidget::handleSelection() {
    int currentRow = tableWidget->currentRow();
    if (currentRow < 0) return;
    
    // Recuperiamo l'ID dall'elemento della prima colonna della riga selezionata
    QString id = tableWidget->item(currentRow, 0)->text();
    
    // Cerchiamo l'oggetto corrispondente nel vettore della MainWindow
    for (Abstract_Activity* activity : mainWindow->getActivityList()) {
        if (activity && activity->getId() == id) {
            emit activitySelected(activity); // Comunica alla MainWindow quale attività è attualmente selezionata
            break;
        }
    }
}

// Slot attivato con il doppio click su una cella: apre direttamente la vista Dettaglio
void ActivityListWidget::handleCellDoubleClick(int row, int column) {
    Q_UNUSED(column); // Evita il warning di variabile non usata
    if (row < 0) return;
    
    QString id = tableWidget->item(row, 0)->text();
    for (Abstract_Activity* activity : mainWindow->getActivityList()) {
        if (activity && activity->getId() == id) {
            // Chiede alla MainWindow di passare alla schermata di dettaglio per questo specifico oggetto
            mainWindow->showActivityDetails(activity);
            break;
        }
    }
}

// Slot scattato al cambio filtro di visualizzazione ("Tutte" vs "Solo Urgenti")
void ActivityListWidget::changeViewMode(int index) {
    Q_UNUSED(index);
    updateActivityList(); // Svuota e rinfresca la tabella applicando i nuovi criteri
}

// Slot scattato al cambio filtro della categoria di classe
void ActivityListWidget::filterByType(int index) {
    Q_UNUSED(index);
    updateActivityList(); // Svuota e rinfresca la tabella applicando i nuovi criteri
}
