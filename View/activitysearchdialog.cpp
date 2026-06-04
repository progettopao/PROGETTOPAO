#include "activitysearchdialog.h"
#include "mainwindow.h"
#include <QHeaderView>
#include <QMessageBox>

#include "bill.h"
#include "hometask.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

ActivitySearchDialog::ActivitySearchDialog(const QVector<Abstract_Activity*>& activities, MainWindow *parent)
    : QDialog(parent), mainWindow(parent), allActivities(activities) {

    setWindowTitle("Cerca Attività");
    resize(600, 400);

    setupUI();
    setupConnections();

    handleSearch("");
}

void ActivitySearchDialog::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // BARRA DI RICERCA SUPERIORE
    QHBoxLayout *searchLayout = new QHBoxLayout();
    searchLayout->addWidget(new QLabel("Cerca per titolo/descrizione:", this));

    searchLineEdit = new QLineEdit(this);
    searchLineEdit->setPlaceholderText("Digita qualcosa per filtrare...");
    searchLineEdit->setClearButtonEnabled(true); // Aggiunge la "X" per svuotare la barra al volo
    searchLayout->addWidget(searchLineEdit);

    mainLayout->addLayout(searchLayout);

    // TABELLA DEI RISULTATI
    resultsTable = new QTableWidget(this);
    resultsTable->setColumnCount(4);
    QStringList headers;
    headers << "ID" << "Titolo" << "Categoria" << "Stato";
    resultsTable->setHorizontalHeaderLabels(headers);

    // Impostazioni tabella
    resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultsTable->setSelectionMode(QAbstractItemView::SingleSelection);
    resultsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    mainLayout->addWidget(resultsTable);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    viewDetailsButton = new QPushButton("Visualizza Dettaglio", this);
    closeButton = new QPushButton("Chiudi", this);

    buttonLayout->addWidget(viewDetailsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(closeButton);

    mainLayout->addLayout(buttonLayout);
}

void ActivitySearchDialog::setupConnections() {
    connect(searchLineEdit, &QLineEdit::textChanged, this, &ActivitySearchDialog::handleSearch);

    // Connessione pulsanti
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(viewDetailsButton, &QPushButton::clicked, this, &ActivitySearchDialog::handleViewDetails);

    // Doppio click su una riga apre direttamente i dettagli
    connect(resultsTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int col) {
        Q_UNUSED(row); Q_UNUSED(col);
        handleViewDetails();
    });
}


void ActivitySearchDialog::handleSearch(const QString &text) {
    filteredActivities.clear();
    QString query = text.trimmed().toLower();

    for (Abstract_Activity *activity : allActivities) {
        if (!activity) continue;
        if (query.isEmpty() ||
            activity->getTitolo().toLower().contains(query) ||
            activity->getDescrizione().toLower().contains(query)) {

            filteredActivities.append(activity); // Inserisce nel sotto-vettore locale
        }
    }

    populateTable();
}

void ActivitySearchDialog::populateTable() {
    resultsTable->setRowCount(0); // Svuota i vecchi record

    for (int i = 0; i < filteredActivities.size(); ++i) {
        Abstract_Activity *activity = filteredActivities[i];
        resultsTable->insertRow(i);

        QString categoria = "Generica";
        if (dynamic_cast<Bill*>(activity))                   categoria = "Bill";
        else if (dynamic_cast<HomeTask*>(activity))           categoria = "HomeTask";
        else if (dynamic_cast<VehicleMaintenance*>(activity)) categoria = "VehicleMaintenance";
        else if (dynamic_cast<ShoppingTask*>(activity))       categoria = "ShoppingTask";
        else if (dynamic_cast<LeisureTimeTask*>(activity))    categoria = "LeisureTimeTask";

        QString stato = activity->isCompletata() ? "Completata" : "In Corso";

        resultsTable->setItem(i, 0, new QTableWidgetItem(activity->getId()));
        resultsTable->setItem(i, 1, new QTableWidgetItem(activity->getTitolo()));
        resultsTable->setItem(i, 2, new QTableWidgetItem(categoria));
        resultsTable->setItem(i, 3, new QTableWidgetItem(stato));
    }
}

// Permette di visualizzare i dettagli dell'elemento selezionato nella ricerca
void ActivitySearchDialog::handleViewDetails() {
    int currentRow = resultsTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "Nessuna selezione", "Seleziona un'attività dalla tabella per vederne i dettagli.");
        return;
    }
    Abstract_Activity *selectedActivity = filteredActivities[currentRow];

    if (selectedActivity && mainWindow) {
        mainWindow->showActivityDetails(selectedActivity); // Dice alla finestra principale di mostrare la scheda dettaglio
        accept(); // Chiude la dialog di ricerca
    }
}
