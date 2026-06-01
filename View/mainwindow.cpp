#include "mainwindow.h" //definizione della classe
#include "activitylistwidget.h" //componenti grafiche
#include "activitydetailwidget.h"
#include "activitysearchdialog.h"

// Classi concrete e derivate da Abstract_Activity
#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

//librerie di Qt
#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QPushButton>
#include <QFormLayout>
#include <QDateTime>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QStatusBar>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentFilePath("") { //costruttore
    setupUI(); //corpo del costruttore
    setupMenus();
    setupConnections();
    updateWindowTitle();
}

//distruttore
MainWindow::~MainWindow() {
    // Svuota la memoria
    clearActivities();
}

void MainWindow::clearActivities() {
    for (Abstract_Activity *activity : activityList) {
        delete activity; // Chiamata polimorfica al distruttore virtuale
    }
    activityList.clear(); //svuota il vettore
}

//GUI principale.
void MainWindow::setupUI() {
    // Creazione dello stack centrale cioè crea il contenitore delle schermate
    centralStack = new QStackedWidget(this); //contenitore delle schermate
    setCentralWidget(centralStack); //Imposta il widget centrale

    // Creazione schermate
    //LISTA
    listWidget = new ActivityListWidget(this);
    ////Inserimento nello stack DELLA PAGINA LISTA
    centralStack->addWidget(listWidget);
    //DETTAGLIO
    detailWidget = new ActivityDetailWidget(this);
    //Inserimento nello stack DELLA PAGINA DETTAGLIO
    centralStack->addWidget(detailWidget);

    centralStack->setCurrentWidget(listWidget); //PAGINA INIZIALE

    statusBar()->showMessage("Pronto"); //STATUS
    resize(850, 600); //DIMENSIONE DELLA FINESTRA
}

//MENU E TOOLBAR
void MainWindow::setupMenus() {
    // Menu File
    QMenu *fileMenu = menuBar()->addMenu("&File"); 
    
    QAction *newAct = fileMenu->addAction("&Nuovo", this, &MainWindow::newLibrary);
    newAct->setShortcut(QKeySequence::New);
    
    QAction *openAct = fileMenu->addAction("&Apri...", this, &MainWindow::openLibrary);
    openAct->setShortcut(QKeySequence::Open);
    
    QAction *saveAct = fileMenu->addAction("&Salva", this, &MainWindow::saveLibrary);
    saveAct->setShortcut(QKeySequence::Save);
    
    QAction *saveAsAct = fileMenu->addAction("Salva con &nome...", this, &MainWindow::saveLibraryAs);
    saveAsAct->setShortcut(QKeySequence::SaveAs);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction("&Esci", this, &QWidget::close);
    exitAct->setShortcut(QKeySequence::Quit);
// MENU MODIFICA (Rimosse emoticon stringhe)
    QMenu *editMenu = menuBar()->addMenu("&Modifica");
    
    QAction *addAct = editMenu->addAction("&Aggiungi Attivita...", this, &MainWindow::addActivity);
    addAct->setShortcut(QKeySequence("Ctrl+N"));

    QAction *searchAct = editMenu->addAction("&Cerca...", this, &MainWindow::search);
    searchAct->setShortcut(QKeySequence::Find);

    // MENU AIUTO
    QMenu *helpMenu = menuBar()->addMenu("&Aiuto");
    QAction *aboutAct = helpMenu->addAction("&Info sull'applicazione", this, &MainWindow::about);

    // Barra degli strumenti
    toolbar = addToolBar("Main Toolbar");
    toolbar->addAction(newAct); 
    toolbar->addAction(openAct); 
    toolbar->addAction(saveAct); 
    toolbar->addSeparator();
    toolbar->addAction(addAct); 
    toolbar->addAction(searchAct); 
}

void MainWindow::setupConnections() {
    // Connessione dei segnali provenienti dal ListWidget
    connect(listWidget, &ActivityListWidget::activitySelected,
            this, &MainWindow::showActivityDetails);
    connect(listWidget, &ActivityListWidget::editRequested,
            this, &MainWindow::editActivity);
    connect(listWidget, &ActivityListWidget::removeRequested,
            this, &MainWindow::removeActivity);

    // Connessione dei segnali provenienti dal DetailWidget
    connect(detailWidget, &ActivityDetailWidget::backRequested,
            this, &MainWindow::showActivityList);
    connect(detailWidget, &ActivityDetailWidget::editRequested,
            this, &MainWindow::editActivity);
            
    connect(detailWidget, &ActivityDetailWidget::deleteRequested,
            this, &MainWindow::removeActivity);
}

//Gestisce il titolo finestra
void MainWindow::updateWindowTitle() {
    QString title = "Task Organizer Pro"; //TITOLO BASE
    if (!currentFilePath.isEmpty()) {
        QFileInfo fileInfo(currentFilePath);
        title += " - " + fileInfo.fileName(); //SE FILE APERTO
    }
    setWindowTitle(title); //aggiorna la finestra reale
}

QVector<Abstract_Activity*>& MainWindow::getActivityList() {
    return activityList; //restituisce la lista originale per riferimento
}

void MainWindow::newLibrary() { // crea archivio vuoto
    if (!activityList.isEmpty()) {
        //mostra un popup
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Nuovo Archivio", "Sei sicuro di voler creare un nuovo archivio? Le modifiche non salvate andranno perse.",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return; //interrompe funzione
        }
    }

    clearActivities(); //Elimina tutte le attività dalla memoria.
    currentFilePath = ""; //Reset file corrente
    updateWindowTitle();
    listWidget->updateActivityList(); //refresh GUI
    showActivityList(); //Torna schermata lista

    statusBar()->showMessage("Nuovo archivio creato", 3000);
}

void MainWindow::openLibrary() {
    if (!activityList.isEmpty()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                                  "Apri Archivio", "Sei sicuro di voler aprire un nuovo file? Le modifiche correnti andranno perse.",
                                                                  QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::No) {
            return;
        }
    }

    // Vincolo 13: Dialog per selezionare dinamicamente file JSON o XML
    QString filePath = QFileDialog::getOpenFileName(this, "Apri Archivio", "", "File di interscambio (*.json *.xml);;JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) {
        return;
    }

    clearActivities();

    // Intercettazione dinamica del formato del file caricato
    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();

    if (ext == "json") {
        activityList = jsonPersistence.loadActivities(filePath);
    } else if (ext == "xml") {
        activityList = xmlPersistence.loadActivities(filePath);
    }

    currentFilePath = filePath;
    updateWindowTitle();
    listWidget->updateActivityList();
    showActivityList();

    statusBar()->showMessage("Archivio caricato con successo", 3000);
}

void MainWindow::saveLibrary() {
    if (currentFilePath.isEmpty()) {
        saveLibraryAs();
        return;
    }

    QFileInfo fileInfo(currentFilePath);
    QString ext = fileInfo.suffix().toLower();
    bool success = false;

    // Salva nel rispettivo formato in base all'estensione del file corrente
    if (ext == "json") {
        success = jsonPersistence.saveActivities(currentFilePath, activityList);
    } else if (ext == "xml") {
        success = xmlPersistence.saveActivities(currentFilePath, activityList);
    }

    if (success) {
        statusBar()->showMessage("Archivio salvato correttamente", 3000);
    } else {
        QMessageBox::warning(this, "Errore di Salvataggio", "Impossibile salvare i dati nel file corrente.");
    }
}

void MainWindow::saveLibraryAs() {
    QString filePath = QFileDialog::getSaveFileName(this, "Salva con nome", "", "JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) {
        return;
    }

    QFileInfo fileInfo(filePath);
    QString ext = fileInfo.suffix().toLower();
    bool success = false;

    if (ext == "json") {
        success = jsonPersistence.saveActivities(filePath, activityList);
    } else if (ext == "xml") {
        success = xmlPersistence.saveActivities(filePath, activityList);
    }

    if (success) {
        currentFilePath = filePath;
        updateWindowTitle();
        statusBar()->showMessage("Archivio salvato con successo", 3000);
    } else {
        QMessageBox::warning(this, "Errore di Salvataggio", "Impossibile completare il salvataggio.");
    }
}

void MainWindow::addActivity() {
    // Lista delle classi concrete del vostro progetto
    QStringList types;
    types << "Bill" << "HomeTask" << "VehicleMaintenance" << "ShoppingTask" << "LeisureTimeTask";

    bool ok;
    QString type = QInputDialog::getItem(this, "Nuova Attività",
                                         "Seleziona la categoria:", types, 0, false, &ok);
    if (!ok || type.isEmpty()) {
        return;
    }

    // Creiamo la Dialog pop-up generica
    QDialog dialog(this);
    dialog.setWindowTitle("Aggiungi " + type);
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    // --- GENERAZIONE AUTOMATICA ID (Abstract_Activity) ---
    QString newId = QString::number(QDateTime::currentMSecsSinceEpoch());
    QLineEdit *txtId = new QLineEdit(newId, &dialog);

    txtId->setReadOnly(true); // L'utente vede l'ID ma non può modificarlo

    QLineEdit *txtTitolo = new QLineEdit(&dialog);
    QLineEdit *txtDescrizione = new QLineEdit(&dialog);
    QCheckBox *chkCompletata = new QCheckBox("Segna come completata", &dialog);
    formLayout->addRow("ID Attività (Auto):", txtId);
    formLayout->addRow("Titolo:", txtTitolo);
    formLayout->addRow("Descrizione:", txtDescrizione);
    formLayout->addRow("Stato:", chkCompletata);

    // Campi per HomeTask
    QLineEdit *txtStanza = nullptr;
    QSpinBox *spinUrgenza = nullptr;
    // Campi per Bill
    QDoubleSpinBox *spinImporto = nullptr;
    QLineEdit *txtEnte = nullptr;
    QDateEdit *dateScadenza = nullptr;

    // Campi per LeisureTimeTask
    QLineEdit *txtCategoriaSvago = nullptr;
    QSpinBox *spinRelax = nullptr;
    QLineEdit *txtCompagnia = nullptr;

    // Campi per ShoppingTask
    QLineEdit *txtSupermercato = nullptr;
    QDoubleSpinBox *spinBudget = nullptr;
    QListWidget *listProdottiWidget = nullptr;

    // Campi per VehicleMaintenance
    QLineEdit *txtTarga = nullptr;
    QLineEdit *txtOfficina = nullptr;
   QListWidget *listComponentiWidget = nullptr;


    if (type == "HomeTask") {
        txtStanza = new QLineEdit(&dialog);
        spinUrgenza = new QSpinBox(&dialog);
        spinUrgenza->setRange(1, 5);

        formLayout->addRow("Stanza/Ambiente:", txtStanza);
        formLayout->addRow("Livello Urgenza (1-5):", spinUrgenza);

    } else if (type == "Bill") {
        spinImporto = new QDoubleSpinBox(&dialog);
        spinImporto->setMaximum(99999.99);
        spinImporto->setSuffix(" €");
        txtEnte = new QLineEdit(&dialog);
        dateScadenza = new QDateEdit(QDate::currentDate(), &dialog);
        dateScadenza->setCalendarPopup(true);

        formLayout->addRow("Importo bolletta:", spinImporto);
        formLayout->addRow("Ente Erogatore:", txtEnte);
        formLayout->addRow("Data di Scadenza:", dateScadenza);

    } else if (type == "LeisureTimeTask") {
        txtCategoriaSvago = new QLineEdit(&dialog);
        spinRelax = new QSpinBox(&dialog);
        spinRelax->setRange(1, 10);
        txtCompagnia = new QLineEdit(&dialog);

        formLayout->addRow("Categoria Svago:", txtCategoriaSvago);
        formLayout->addRow("Livello Relax (1-10):", spinRelax);
        formLayout->addRow("In compagnia di:", txtCompagnia);

    } else if (type == "ShoppingTask") {
        txtSupermercato = new QLineEdit(&dialog);
        spinBudget = new QDoubleSpinBox(&dialog);
        spinBudget->setMaximum(9999.99);
        spinBudget->setSuffix(" €");

        formLayout->addRow("Negozio/Supermercato:", txtSupermercato);
        formLayout->addRow("Budget Massimo:", spinBudget);

        listProdottiWidget = new QListWidget(&dialog);
        listProdottiWidget->setMaximumHeight(100);

        QLineEdit *txtNuovoProdotto = new QLineEdit(&dialog);
        txtNuovoProdotto->setPlaceholderText("Scrivi prodotto e premi Aggiungi...");
        QPushButton *btnAggiungiProdotto = new QPushButton("Aggiungi", &dialog);

        QHBoxLayout *prodottoInputLayout = new QHBoxLayout();
        prodottoInputLayout->addWidget(txtNuovoProdotto);
        prodottoInputLayout->addWidget(btnAggiungiProdotto);

        connect(btnAggiungiProdotto, &QPushButton::clicked, this, [txtNuovoProdotto, listProdottiWidget]() {
            QString prodText = txtNuovoProdotto->text().trimmed();
            if (!prodText.isEmpty()) {
                listProdottiWidget->addItem(prodText);
                txtNuovoProdotto->clear();
                txtNuovoProdotto->setFocus();
            }
        });

        formLayout->addRow("Lista Prodotti:", listProdottiWidget);
        formLayout->addRow("", prodottoInputLayout);

    } else if (type == "VehicleMaintenance") {
        txtTarga = new QLineEdit(&dialog);
        txtOfficina = new QLineEdit(&dialog);
        
        formLayout->addRow("Targa Veicolo:", txtTarga);
        formLayout->addRow("Officina di Riferimento:", txtOfficina);

        // --- INTERFACCIA DINAMICA PER I COMPONENTI VEICOLO ---
        listComponentiWidget = new QListWidget(&dialog);
        listComponentiWidget->setMaximumHeight(100);

        QLineEdit *txtNuovoComponente = new QLineEdit(&dialog);
        txtNuovoComponente->setPlaceholderText("Es. Olio, Pastiglie freni...");
        QPushButton *btnAggiungiComponente = new QPushButton("Aggiungi", &dialog);

        QHBoxLayout *componenteInputLayout = new QHBoxLayout();
        componenteInputLayout->addWidget(txtNuovoComponente);
        componenteInputLayout->addWidget(btnAggiungiComponente);

        // Collegamento per inserire dinamicamente i pezzi nella lista
        connect(btnAggiungiComponente, &QPushButton::clicked, this, [txtNuovoComponente, listComponentiWidget]() {
            QString compText = txtNuovoComponente->text().trimmed();
            if (!compText.isEmpty()) {
                listComponentiWidget->addItem(compText);
                txtNuovoComponente->clear();
                txtNuovoComponente->setFocus();
            }
        });

        formLayout->addRow("Componenti da Sostituire:", listComponentiWidget);
        formLayout->addRow("", componenteInputLayout);
    }

    mainLayout->addLayout(formLayout);

    // --- PULSANTIERA DI CONFERMA ---
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Salva", &dialog);
    QPushButton *cancelButton = new QPushButton("Annulla", &dialog);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);

    connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // 4. ESECUZIONE DELLA DIALOG E COSTRUZIONE DELL'OGGETTO
    if (dialog.exec() == QDialog::Accepted) {
        if (txtTitolo->text().isEmpty()) {
            QMessageBox::warning(this, "Errore di inserimento", "Il campo 'Titolo' non può essere vuoto!");
            return;
        }

        bool isCompletata = chkCompletata->isChecked();
        Abstract_Activity *newAct = nullptr;

        if (type == "HomeTask") {
            newAct = new HomeTask(newId, txtTitolo->text(), txtDescrizione->text(), isCompletata,
                                  txtStanza->text(), spinUrgenza->value());

        } else if (type == "Bill") {
            newAct = new Bill(newId, txtTitolo->text(), txtDescrizione->text(), isCompletata,
                              spinImporto->value(), txtEnte->text(), dateScadenza->date());

        } else if (type == "LeisureTimeTask") {
            newAct = new LeisureTimeTask(newId, txtTitolo->text(), txtDescrizione->text(), isCompletata,
                                         txtCategoriaSvago->text(), spinRelax->value(), txtCompagnia->text());

        } else if (type == "ShoppingTask") {
            ShoppingTask *shopAct = new ShoppingTask(newId, txtTitolo->text(), txtDescrizione->text(), isCompletata,
                                                     txtSupermercato->text(), spinBudget->value());
            if (listProdottiWidget) {
                for (int i = 0; i < listProdottiWidget->count(); ++i) {
                    shopAct->aggiungiArticolo(listProdottiWidget->item(i)->text());
                }
            }
            newAct = shopAct;

        } else if (type == "VehicleMaintenance") {
            // 1. Creiamo il task manutenzione auto con i campi base
            VehicleMaintenance *vMaintAct = new VehicleMaintenance(newId, txtTitolo->text(), txtDescrizione->text(), isCompletata,
                                                                   txtTarga->text(), txtOfficina->text());
            // 2. Estraiamo i pezzi scritti nella lista grafica e li salviamo dentro l'oggetto
            if (listComponentiWidget) {
                for (int i = 0; i < listComponentiWidget->count(); ++i) {
                    vMaintAct->aggiungiComponente(listComponentiWidget->item(i)->text());
                }
            }
            newAct = vMaintAct;
        }

        // 5. AGGIORNAMENTO DEL VETTORE E DELLA TABELLA
        if (newAct) {
            activityList.append(newAct);
            listWidget->updateActivityList();
            statusBar()->showMessage(type + " aggiunta con successo!", 3000);
        }
    }
}

void MainWindow::editActivity(Abstract_Activity *activity) {
    if (!activity) return;

    QDialog dialog(this);
    dialog.setWindowTitle("Modifica Attività");
    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);
    QFormLayout *formLayout = new QFormLayout();

    QLineEdit *txtId = new QLineEdit(activity->getId(), &dialog);
    txtId->setReadOnly(true); // L'ID non deve mai essere modificabile
    txtId->setStyleSheet("background-color: #e9ecef; color: #495057; font-weight: bold;");

    QLineEdit *txtTitolo = new QLineEdit(activity->getTitolo(), &dialog);
    QLineEdit *txtDescrizione = new QLineEdit(activity->getDescrizione(), &dialog);
    QCheckBox *chkCompletata = new QCheckBox("Segna come completata", &dialog);
    chkCompletata->setChecked(activity->isCompletata());

    formLayout->addRow("ID Attività (Bloccato):", txtId);
    formLayout->addRow("Titolo:", txtTitolo);
    formLayout->addRow("Descrizione:", txtDescrizione);
    formLayout->addRow("Stato:", chkCompletata);

    // --- DICHIARAZIONE DEI PUNTATORI PER I CAMPI SPECIFICI ---
    QLineEdit *txtStanza = nullptr; QSpinBox *spinUrgenza = nullptr;                                           // HomeTask
    QDoubleSpinBox *spinImporto = nullptr; QLineEdit *txtEnte = nullptr; QDateEdit *dateScadenza = nullptr; // Bill
    QLineEdit *txtCategoriaSvago = nullptr; QSpinBox *spinRelax = nullptr; QLineEdit *txtCompagnia = nullptr; // LeisureTimeTask
    QLineEdit *txtSupermercato = nullptr; QDoubleSpinBox *spinBudget = nullptr; QListWidget *listProdottiWidget = nullptr; // ShoppingTask
    QLineEdit *txtTarga = nullptr; QLineEdit *txtOfficina = nullptr; QListWidget *listComponentiWidget = nullptr; // VehicleMaintenance

    // --- 1. RICONOSCIMENTO DEL TIPO E POPOLAMENTO DATI TRAMITE DYNAMIC_CAST ---
    if (HomeTask *homeAct = dynamic_cast<HomeTask*>(activity)) {
        txtStanza = new QLineEdit(homeAct->getStanza(), &dialog);
        spinUrgenza = new QSpinBox(&dialog);
        spinUrgenza->setRange(1, 5);
        spinUrgenza->setValue(homeAct->getUrgenza());

        formLayout->addRow("Stanza/Ambiente:", txtStanza);
        formLayout->addRow("Livello Urgenza (1-5):", spinUrgenza);

    } else if (Bill *billAct = dynamic_cast<Bill*>(activity)) {
        spinImporto = new QDoubleSpinBox(&dialog);
        spinImporto->setMaximum(99999.99);
        spinImporto->setSuffix(" €");
        spinImporto->setValue(billAct->getImporto());

        txtEnte = new QLineEdit(billAct->getEnteErogatore(), &dialog);
        dateScadenza = new QDateEdit(billAct->getDataScadenza(), &dialog);
        dateScadenza->setCalendarPopup(true);

        formLayout->addRow("Importo bolletta:", spinImporto);
        formLayout->addRow("Ente Erogatore:", txtEnte);
        formLayout->addRow("Data di Scadenza:", dateScadenza);

    } else if (LeisureTimeTask *leisureAct = dynamic_cast<LeisureTimeTask*>(activity)) {
        txtCategoriaSvago = new QLineEdit(leisureAct->getCategoriaSvago(), &dialog);
        spinRelax = new QSpinBox(&dialog);
        spinRelax->setRange(1, 10);
        spinRelax->setValue(leisureAct->getLivelloRelax());
        txtCompagnia = new QLineEdit(leisureAct->getCompagnia(), &dialog);

        formLayout->addRow("Categoria Svago:", txtCategoriaSvago);
        formLayout->addRow("Livello Relax (1-10):", spinRelax);
        formLayout->addRow("In compagnia di:", txtCompagnia);

    } else if (ShoppingTask *shopAct = dynamic_cast<ShoppingTask*>(activity)) {
        txtSupermercato = new QLineEdit(shopAct->getSupermercatoConsigliato(), &dialog);
        spinBudget = new QDoubleSpinBox(&dialog);
        spinBudget->setMaximum(9999.99);
        spinBudget->setSuffix(" €");
        spinBudget->setValue(shopAct->getBudgetMassimo());

        formLayout->addRow("Negozio/Supermercato:", txtSupermercato);
        formLayout->addRow("Budget Massimo:", spinBudget);

        listProdottiWidget = new QListWidget(&dialog);
        listProdottiWidget->setMaximumHeight(100);
        listProdottiWidget->addItems(shopAct->getListaProdotti()); // Carica i prodotti già salvati

        QLineEdit *txtNuovoProdotto = new QLineEdit(&dialog);
        txtNuovoProdotto->setPlaceholderText("Aggiungi prodotto...");
        QPushButton *btnAggiungi = new QPushButton("Aggiungi", &dialog);
        QHBoxLayout *layProd = new QHBoxLayout();
        layProd->addWidget(txtNuovoProdotto); layProd->addWidget(btnAggiungi);

        connect(btnAggiungi, &QPushButton::clicked, this, [txtNuovoProdotto, listProdottiWidget]() {
            if (!txtNuovoProdotto->text().trimmed().isEmpty()) {
                listProdottiWidget->addItem(txtNuovoProdotto->text().trimmed());
                txtNuovoProdotto->clear();
            }
        });
        formLayout->addRow("Lista Prodotti:", listProdottiWidget);
        formLayout->addRow("", layProd);

    } else if (VehicleMaintenance *vAct = dynamic_cast<VehicleMaintenance*>(activity)) {
        txtTarga = new QLineEdit(vAct->getTargaVeicolo(), &dialog);
        txtOfficina = new QLineEdit(vAct->getOfficinaRiferimento(), &dialog);
        

        formLayout->addRow("Targa Veicolo:", txtTarga);
        formLayout->addRow("Officina di Riferimento:", txtOfficina);

        listComponentiWidget = new QListWidget(&dialog);
        listComponentiWidget->setMaximumHeight(100);
        listComponentiWidget->addItems(vAct->getComponentiDaSostituire()); // Carica i vecchi componenti

        QLineEdit *txtNuovoComp = new QLineEdit(&dialog);
        txtNuovoComp->setPlaceholderText("Aggiungi componente...");
        QPushButton *btnAggiungiComp = new QPushButton("Aggiungi", &dialog);
        QHBoxLayout *layComp = new QHBoxLayout();
        layComp->addWidget(txtNuovoComp); layComp->addWidget(btnAggiungiComp);

        connect(btnAggiungiComp, &QPushButton::clicked, this, [txtNuovoComp, listComponentiWidget]() {
            if (!txtNuovoComp->text().trimmed().isEmpty()) {
                listComponentiWidget->addItem(txtNuovoComp->text().trimmed());
                txtNuovoComp->clear();
            }
        });
        formLayout->addRow("Componenti da Sostituire:", listComponentiWidget);
        formLayout->addRow("", layComp);
    }

    mainLayout->addLayout(formLayout);

    QPushButton *saveButton = new QPushButton("Salva");
    QPushButton *cancelButton = new QPushButton("Annulla");

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout); // Corretto da layout-> a mainLayout->

    connect(saveButton, &QPushButton::clicked, &dialog, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, &dialog, &QDialog::reject);

    // --- 2. SALVATAGGIO DELLE MODIFICHE SULL'OGGETTO ESISTENTE ---
    if (dialog.exec() == QDialog::Accepted) {
        if (txtTitolo->text().isEmpty()) {
            QMessageBox::warning(this, "Errore", "Il titolo non può essere vuoto!");
            return;
        }

        // Aggiorna i campi base ereditati
        activity->setTitolo(txtTitolo->text());
        activity->setDescrizione(txtDescrizione->text());
        activity->setCompletata(chkCompletata->isChecked());

        // Aggiorna i campi specifici usando lo stesso dynamic_cast sicuro
        if (HomeTask *homeAct = dynamic_cast<HomeTask*>(activity)) {
            homeAct->setStanza(txtStanza->text());
            homeAct->setUrgenza(spinUrgenza->value());

        } else if (Bill *billAct = dynamic_cast<Bill*>(activity)) {
            billAct->setImporto(spinImporto->value());
            billAct->setEnteErogatore(txtEnte->text());
            billAct->setDataScadenza(dateScadenza->date());

        } else if (LeisureTimeTask *leisureAct = dynamic_cast<LeisureTimeTask*>(activity)) {
            leisureAct->setCategoriaSvago(txtCategoriaSvago->text());
            leisureAct->setLivelloRelax(spinRelax->value());
            leisureAct->setCompagnia(txtCompagnia->text());

        } else if (ShoppingTask *shopAct = dynamic_cast<ShoppingTask*>(activity)) {
            shopAct->setSupermercatoConsigliato(txtSupermercato->text());
            shopAct->setBudgetMassimo(spinBudget->value());
        }
        else if (VehicleMaintenance *vAct = dynamic_cast<VehicleMaintenance*>(activity)) {
            vAct->setTargaVeicolo(txtTarga->text());
            vAct->setOfficinaRiferimento(txtOfficina->text());
            
        }

        // Rinfresca la grafica
        listWidget->updateActivityList();
        if (centralStack->currentWidget() == detailWidget) {
            showActivityDetails(activity); // Aggiorna anche la pagina dei dettagli
        }
        statusBar()->showMessage("Attività aggiornata con successo", 3000);
    }
}


void MainWindow::removeActivity(Abstract_Activity *activity) {
    if (!activity) return;

    QMessageBox::StandardButton reply = QMessageBox::question(this,
                                                              "Elimina Attività", "Sei sicuro di voler rimuovere l'attività '" + activity->getTitolo() + "'?",
                                                              QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::No) {
        return;
    }

    int index = activityList.indexOf(activity);
    if (index != -1) {
        activityList.removeAt(index);
        delete activity; // Distrugge l'istanza dall'Heap ed evita Memory Leak

        listWidget->updateActivityList();
        showActivityList(); // Riporta l'utente alla tabella principale

        statusBar()->showMessage("Attività rimossa con successo", 3000);
    }
}

void MainWindow::showActivityDetails(Abstract_Activity *activity) {
    if (!activity) return;

    detailWidget->setActivity(activity);
    centralStack->setCurrentWidget(detailWidget);
}

void MainWindow::showActivityList() {
    centralStack->setCurrentWidget(listWidget);
}

void MainWindow::search() {
    ActivitySearchDialog dialog(activityList, this);
    dialog.exec();
}

void MainWindow::about() {
    QMessageBox::about(this, "Info su HomeTask Manager",
                       "HomeTask Manager 1.0\n\n"
                       "Applicazione ad oggetti per la gestione e pianificazione delle scadenze quotidiane.\n\n"
                       "Sviluppato in C++ e Qt per il corso di Programmazione Oggetti.");
}
