#include "mainwindow.h"
#include "activitylistwidget.h"
#include "activitydetailwidget.h"
#include "activitysearchdialog.h"

#include "hometask.h"
#include "bill.h"
#include "vehiclemaintenance.h"
#include "shoppingtask.h"
#include "leisuretimetask.h"

#include <QMenuBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QApplication>
#include <QFileInfo>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QDateTime>
#include <QStatusBar>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QStringList>
#include <QDateEdit>
#include <QStyle>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), currentFilePath(""), activityUnderEdit(nullptr), currentNewType(""), toolbar(nullptr) {
    setupUI();
    setupMenus();
    setupConnections();
    updateWindowTitle();
}

MainWindow::~MainWindow() {
    clearActivities();
}

void MainWindow::clearActivities() {
    for (Abstract_Activity *activity : activityList) {
        delete activity;
    }
    activityList.clear();
}

void MainWindow::setupUI() {
    QWidget *mainCentralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainCentralWidget);
    mainLayout->setContentsMargins(5, 5, 5, 5);
    mainLayout->setSpacing(5);

    centralStack = new QStackedWidget(this);
    listWidget = new ActivityListWidget(this);
    detailWidget = new ActivityDetailWidget(this);
    centralStack->addWidget(listWidget);
    centralStack->addWidget(detailWidget);
    centralStack->setCurrentWidget(listWidget);
    mainLayout->addWidget(centralStack, 1);

    formContainer = new QWidget(this);
    formContainer->setStyleSheet("background-color: #f8f9fa; border-top: 1px solid #dee2e6;");
    QVBoxLayout *containerLayout = new QVBoxLayout(formContainer);

    QLabel *formTitle = new QLabel("<b>Gestione Attività</b>", formContainer);
    containerLayout->addWidget(formTitle);

    formLayout = new QFormLayout();

    cmbCategoria = new QComboBox(formContainer);
    cmbCategoria->addItems(QStringList() << "Bill" << "HomeTask" << "VehicleMaintenance" << "ShoppingTask" << "LeisureTimeTask");
    formLayout->addRow("Categoria:", cmbCategoria);

    txtTitolo = new QLineEdit(formContainer);
    txtDescrizione = new QLineEdit(formContainer);
    chkCompletata = new QCheckBox("Segna come completata", formContainer);

    formLayout->addRow("Titolo:", txtTitolo);
    formLayout->addRow("Descrizione:", txtDescrizione);
    formLayout->addRow("Stato:", chkCompletata);
    containerLayout->addLayout(formLayout);

    formTypeStack = new QStackedWidget(formContainer);

    // --- Pagina 0: Bill ---
    QWidget *pageBill = new QWidget(this);
    QFormLayout *layoutBill = new QFormLayout(pageBill);
    layoutBill->setContentsMargins(0,0,0,0);
    dspImporto = new QDoubleSpinBox(pageBill);
    dspImporto->setMaximum(99999.99);
    txtUtenza = new QLineEdit(pageBill);
    datScadenza = new QDateEdit(QDate::currentDate(), pageBill);
    datScadenza->setCalendarPopup(true);
    layoutBill->addRow("Importo (€):", dspImporto);
    layoutBill->addRow("Ente Erogatore:", txtUtenza);
    layoutBill->addRow("Scadenza:", datScadenza);
    formTypeStack->addWidget(pageBill);

    // --- Pagina 1: HomeTask ---
    QWidget *pageHome = new QWidget(this);
    QFormLayout *layoutHome = new QFormLayout(pageHome);
    layoutHome->setContentsMargins(0,0,0,0);
    txtStanza = new QLineEdit(pageHome);
    spnUrgenza = new QSpinBox(pageHome);
    spnUrgenza->setRange(1, 5);
    layoutHome->addRow("Stanza:", txtStanza);
    layoutHome->addRow("Urgenza (1-5):", spnUrgenza);
    formTypeStack->addWidget(pageHome);

    // --- Pagina 2: VehicleMaintenance ---
    QWidget *pageVehicle = new QWidget(this);
    QFormLayout *layoutVehicle = new QFormLayout(pageVehicle);
    layoutVehicle->setContentsMargins(0,0,0,0);
    txtVeicolo = new QLineEdit(pageVehicle);
    txtTipoIntervento = new QLineEdit(pageVehicle);
    txtComponenti = new QLineEdit(pageVehicle);
    txtComponenti->setPlaceholderText("Es: Olio, Filtro, Pastiglie (separati da virgola)");
    layoutVehicle->addRow("Targa Veicolo:", txtVeicolo);
    layoutVehicle->addRow("Officina Rif.:", txtTipoIntervento);
    layoutVehicle->addRow("Componenti:", txtComponenti);
    formTypeStack->addWidget(pageVehicle);

    // --- Pagina 3: ShoppingTask ---
    QWidget *pageShopping = new QWidget(this);
    QFormLayout *layoutShopping = new QFormLayout(pageShopping);
    layoutShopping->setContentsMargins(0,0,0,0);
    txtNegozio = new QLineEdit(pageShopping);
    dspBudget = new QDoubleSpinBox(pageShopping);
    dspBudget->setMaximum(9999.99);
    txtProdotti = new QLineEdit(pageShopping);
    txtProdotti->setPlaceholderText("Es: Pane, Latte, Uova (separati da virgola)");
    layoutShopping->addRow("Supermercato:", txtNegozio);
    layoutShopping->addRow("Budget Massimo (€):", dspBudget);
    layoutShopping->addRow("Lista Prodotti:", txtProdotti);
    formTypeStack->addWidget(pageShopping);

    // --- Pagina 4: LeisureTimeTask ---
    QWidget *pageLeisure = new QWidget(this);
    QFormLayout *layoutLeisure = new QFormLayout(pageLeisure);
    layoutLeisure->setContentsMargins(0,0,0,0);
    txtTipoSvago = new QLineEdit(pageLeisure);
    spnPriorita = new QSpinBox(pageLeisure);
    txtCompagnia = new QLineEdit(pageLeisure);
    layoutLeisure->addRow("Categoria Svago:", txtTipoSvago);
    layoutLeisure->addRow("Livello Relax:", spnPriorita);
    layoutLeisure->addRow("In Compagnia di:", txtCompagnia);
    formTypeStack->addWidget(pageLeisure);

    containerLayout->addWidget(formTypeStack);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    QPushButton *saveButton = new QPushButton("Salva", formContainer);
    QPushButton *cancelButton = new QPushButton("Annulla", formContainer);
    buttonLayout->addStretch();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(cancelButton);
    containerLayout->addLayout(buttonLayout);

    mainLayout->addWidget(formContainer);
    formContainer->hide();

    setCentralWidget(mainCentralWidget);
    statusBar()->showMessage("Pronto");
    resize(850, 650);

    connect(saveButton, &QPushButton::clicked, this, &MainWindow::saveFormUnder);
    connect(cancelButton, &QPushButton::clicked, this, [this]() {
        formContainer->hide();
        activityUnderEdit = nullptr;
    });

    connect(cmbCategoria, &QComboBox::currentIndexChanged, formTypeStack, &QStackedWidget::setCurrentIndex);
}

void MainWindow::setupMenus() {
    QMenuBar *mainMenuBar = menuBar();
    mainMenuBar->clear();

    // --- MENU FILE ---
    QMenu *fileMenu = mainMenuBar->addMenu("&File");

    QAction *actNuovo = fileMenu->addAction("&Nuovo Archivio");
    actNuovo->setShortcut(QKeySequence::New);
    connect(actNuovo, &QAction::triggered, this, &MainWindow::newLibrary);

    QAction *actApri = fileMenu->addAction("&Apri Archivio...");
    actApri->setShortcut(QKeySequence::Open);
    connect(actApri, &QAction::triggered, this, &MainWindow::openLibrary);

    QAction *actSalva = fileMenu->addAction("&Salva");
    actSalva->setShortcut(QKeySequence::Save);
    connect(actSalva, &QAction::triggered, this, &MainWindow::saveLibrary);

    QAction *actSalvaConNome = fileMenu->addAction("Salva con &nome...");
    actSalvaConNome->setShortcut(QKeySequence::SaveAs);
    connect(actSalvaConNome, &QAction::triggered, this, &MainWindow::saveLibraryAs);

    fileMenu->addSeparator();
    QAction *actEsci = fileMenu->addAction("&Esci");
    actEsci->setShortcut(QKeySequence::Quit);
    connect(actEsci, &QAction::triggered, this, &QWidget::close);

    // --- MENU MODIFICA ---
    QMenu *editMenu = mainMenuBar->addMenu("&Modifica");

    QAction *actAggiungi = editMenu->addAction("&Aggiungi Attività");
    actAggiungi->setShortcut(tr("Ctrl+A"));
    connect(actAggiungi, &QAction::triggered, this, &MainWindow::addActivity);

    QAction *actCerca = editMenu->addAction("&Cerca...");
    actCerca->setShortcut(QKeySequence::Find);
    connect(actCerca, &QAction::triggered, this, &MainWindow::search);

    // --- MENU INFO ---
    QMenu *helpMenu = mainMenuBar->addMenu("&Info");
    QAction *actAbout = helpMenu->addAction("&Informazioni su...");
    connect(actAbout, &QAction::triggered, this, &MainWindow::about);

    // --- TOOLBAR NORMALE SENZA ICONE ---
    if (!toolbar) {
        toolbar = addToolBar("Main Toolbar");
        toolbar->setMovable(false);
        toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);
    } else {
        toolbar->clear();
    }

    toolbar->addAction(actNuovo);
    toolbar->addAction(actApri);
    toolbar->addAction(actSalva);
    toolbar->addSeparator();
    toolbar->addAction(actAggiungi);
    toolbar->addAction(actCerca);
}

void MainWindow::setupConnections() {
    connect(listWidget, &ActivityListWidget::activitySelected, this, &MainWindow::showActivityDetails);
    connect(listWidget, &ActivityListWidget::editRequested, this, &MainWindow::editActivity);
    connect(listWidget, &ActivityListWidget::removeRequested, this, &MainWindow::removeActivity);

    connect(detailWidget, &ActivityDetailWidget::backRequested, this, &MainWindow::showActivityList);
    connect(detailWidget, &ActivityDetailWidget::editRequested, this, &MainWindow::editActivity);
    connect(detailWidget, &ActivityDetailWidget::deleteRequested, this, &MainWindow::removeActivity);
}

void MainWindow::updateWindowTitle() {
    QString title = "Task Organizer Pro";
    if (!currentFilePath.isEmpty()) {
        title += " - " + QFileInfo(currentFilePath).fileName();
    }
    setWindowTitle(title);
}

QVector<Abstract_Activity*>& MainWindow::getActivityList() { return activityList; }
void MainWindow::refreshView() { listWidget->updateActivityList(); }

void MainWindow::newLibrary() {
    clearActivities(); currentFilePath = ""; updateWindowTitle(); refreshView(); showActivityList(); formContainer->hide();
}

void MainWindow::openLibrary() {
    QString filePath = QFileDialog::getOpenFileName(this, "Apri Archivio", "", "JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) return;
    clearActivities();
    if (QFileInfo(filePath).suffix().toLower() == "json") activityList = jsonPersistence.loadActivities(filePath);
    else activityList = xmlPersistence.loadActivities(filePath);
    currentFilePath = filePath; updateWindowTitle(); refreshView(); showActivityList(); formContainer->hide();
}

void MainWindow::saveLibrary() {
    if (currentFilePath.isEmpty()) { saveLibraryAs(); return; }
    if (QFileInfo(currentFilePath).suffix().toLower() == "json") jsonPersistence.saveActivities(currentFilePath, activityList);
    else xmlPersistence.saveActivities(currentFilePath, activityList);
}

void MainWindow::saveLibraryAs() {
    QString filePath = QFileDialog::getSaveFileName(this, "Salva con nome", "", "JSON (*.json);;XML (*.xml)");
    if (filePath.isEmpty()) return;
    if (QFileInfo(filePath).suffix().toLower() == "json") jsonPersistence.saveActivities(filePath, activityList);
    else xmlPersistence.saveActivities(filePath, activityList);
    currentFilePath = filePath; updateWindowTitle();
}

void MainWindow::addActivity() {
    activityUnderEdit = nullptr;

    txtTitolo->clear();
    txtDescrizione->clear();
    chkCompletata->setChecked(false);

    dspImporto->setValue(0.0); txtUtenza->clear(); datScadenza->setDate(QDate::currentDate());
    txtStanza->clear(); spnUrgenza->setValue(1);
    txtVeicolo->clear(); txtTipoIntervento->clear(); txtComponenti->clear();
    txtNegozio->clear(); dspBudget->setValue(0.0); txtProdotti->clear();
    txtTipoSvago->clear(); spnPriorita->setValue(1); txtCompagnia->clear();

    cmbCategoria->setEnabled(true);
    cmbCategoria->setCurrentIndex(0);
    formTypeStack->setCurrentIndex(0);

    formContainer->show();
    txtTitolo->setFocus();
}

void MainWindow::editActivity(Abstract_Activity *activity) {
    if (!activity) return;
    activityUnderEdit = activity;

    txtTitolo->setText(activity->getTitolo());
    txtDescrizione->setText(activity->getDescrizione());
    chkCompletata->setChecked(activity->isCompletata());

    cmbCategoria->setEnabled(false);

    if (Bill *b = dynamic_cast<Bill*>(activity)) {
        cmbCategoria->setCurrentText("Bill");
        dspImporto->setValue(b->getImporto());
        txtUtenza->setText(b->getEnteErogatore());
        datScadenza->setDate(b->getDataScadenza());
    }
    else if (HomeTask *h = dynamic_cast<HomeTask*>(activity)) {
        cmbCategoria->setCurrentText("HomeTask");
        txtStanza->setText(h->getStanza());
        spnUrgenza->setValue(h->getUrgenza());
    }
    else if (VehicleMaintenance *v = dynamic_cast<VehicleMaintenance*>(activity)) {
        cmbCategoria->setCurrentText("VehicleMaintenance");
        txtVeicolo->setText(v->getTargaVeicolo());
        txtTipoIntervento->setText(v->getOfficinaRiferimento());
        txtComponenti->setText(v->getComponentiDaSostituire().join(", "));
    }
    else if (ShoppingTask *s = dynamic_cast<ShoppingTask*>(activity)) {
        cmbCategoria->setCurrentText("ShoppingTask");
        txtNegozio->setText(s->getSupermercatoConsigliato());
        dspBudget->setValue(s->getBudgetMassimo());
        txtProdotti->setText(s->getListaProdotti().join(", "));
    }
    else if (LeisureTimeTask *l = dynamic_cast<LeisureTimeTask*>(activity)) {
        cmbCategoria->setCurrentText("LeisureTimeTask");
        txtTipoSvago->setText(l->getCategoriaSvago());
        spnPriorita->setValue(l->getLivelloRelax());
        txtCompagnia->setText(l->getCompagnia());
    }

    formContainer->show();
    txtTitolo->setFocus();
}

void MainWindow::saveFormUnder() {
    if (txtTitolo->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Errore", "Il campo 'Titolo' è obbligatorio.");
        return;
    }

    if (activityUnderEdit) {
        activityUnderEdit->setTitolo(txtTitolo->text());
        activityUnderEdit->setDescrizione(txtDescrizione->text());
        activityUnderEdit->setCompletata(chkCompletata->isChecked());

        if (Bill *b = dynamic_cast<Bill*>(activityUnderEdit)) {
            b->setImporto(dspImporto->value());
            b->setEnteErogatore(txtUtenza->text());
            b->setDataScadenza(datScadenza->date());
        }
        else if (HomeTask *h = dynamic_cast<HomeTask*>(activityUnderEdit)) {
            h->setStanza(txtStanza->text());
            h->setUrgenza(spnUrgenza->value());
        }
        else if (VehicleMaintenance *v = dynamic_cast<VehicleMaintenance*>(activityUnderEdit)) {
            v->setTargaVeicolo(txtVeicolo->text());
            v->setOfficinaRiferimento(txtTipoIntervento->text());
            QStringList compList;
            for(const QString& comp : txtComponenti->text().split(",", Qt::SkipEmptyParts)) {
                compList.append(comp.trimmed());
            }
            v->setComponentiDaSostituire(compList);
        }
        else if (ShoppingTask *s = dynamic_cast<ShoppingTask*>(activityUnderEdit)) {
            s->setSupermercatoConsigliato(txtNegozio->text());
            s->setBudgetMassimo(dspBudget->value());
            QStringList prodList;
            for(const QString& prod : txtProdotti->text().split(",", Qt::SkipEmptyParts)) {
                prodList.append(prod.trimmed());
            }
            s->setListaProdotti(prodList);
        }
        else if (LeisureTimeTask *l = dynamic_cast<LeisureTimeTask*>(activityUnderEdit)) {
            l->setCategoriaSvago(txtTipoSvago->text());
            l->setLivelloRelax(spnPriorita->value());
            l->setCompagnia(txtCompagnia->text());
        }

    } else {
        QString newId = QString::number(QDateTime::currentMSecsSinceEpoch());
        Abstract_Activity *newAct = nullptr;
        QString selectedType = cmbCategoria->currentText();

        if (selectedType == "Bill") {
            newAct = new Bill(newId, txtTitolo->text(), txtDescrizione->text(), chkCompletata->isChecked(), dspImporto->value(), txtUtenza->text(), datScadenza->date());
        } else if (selectedType == "HomeTask") {
            newAct = new HomeTask(newId, txtTitolo->text(), txtDescrizione->text(), chkCompletata->isChecked(), txtStanza->text(), spnUrgenza->value());
        } else if (selectedType == "VehicleMaintenance") {
            QStringList compList;
            for(const QString& comp : txtComponenti->text().split(",", Qt::SkipEmptyParts)) { compList.append(comp.trimmed()); }
            newAct = new VehicleMaintenance(newId, txtTitolo->text(), txtDescrizione->text(), chkCompletata->isChecked(), txtVeicolo->text(), txtTipoIntervento->text(), compList);
        } else if (selectedType == "ShoppingTask") {
            QStringList prodList;
            for(const QString& prod : txtProdotti->text().split(",", Qt::SkipEmptyParts)) { prodList.append(prod.trimmed()); }
            newAct = new ShoppingTask(newId, txtTitolo->text(), txtDescrizione->text(), chkCompletata->isChecked(), txtNegozio->text(), dspBudget->value(), prodList);
        } else if (selectedType == "LeisureTimeTask") {
            newAct = new LeisureTimeTask(newId, txtTitolo->text(), txtDescrizione->text(), chkCompletata->isChecked(), txtTipoSvago->text(), spnPriorita->value(), txtCompagnia->text());
        }

        if (newAct) activityList.append(newAct);
    }

    refreshView();
    formContainer->hide();
    activityUnderEdit = nullptr;
    if (centralStack->currentWidget() == detailWidget) showActivityList();

    // NOTA: Se un file è già aperto, salva le modifiche in background.
    // Se l'archivio è nuovo di zecca, aggiunge l'attività alla lista senza far comparire pop-up fastidiosi!
    if (!currentFilePath.isEmpty()) {
        if (QFileInfo(currentFilePath).suffix().toLower() == "json") jsonPersistence.saveActivities(currentFilePath, activityList);
        else xmlPersistence.saveActivities(currentFilePath, activityList);
    }
}

void MainWindow::removeActivity(Abstract_Activity *activity) {
    if (!activity) return;
    int reply = QMessageBox::question(this, "Elimina Attività", "Rimuovere '" + activity->getTitolo() + "'?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) return;

    int index = activityList.indexOf(activity);
    if (index != -1) {
        activityList.removeAt(index);
        delete activity;
        refreshView(); showActivityList(); formContainer->hide();
        if (!currentFilePath.isEmpty()) {
            if (QFileInfo(currentFilePath).suffix().toLower() == "json") jsonPersistence.saveActivities(currentFilePath, activityList);
            else xmlPersistence.saveActivities(currentFilePath, activityList);
        }
    }
}

void MainWindow::showActivityDetails(Abstract_Activity *activity) {
    if (!activity) return;
    detailWidget->setActivity(activity);
    centralStack->setCurrentWidget(detailWidget);
    formContainer->hide();
}

void MainWindow::showActivityList() { centralStack->setCurrentWidget(listWidget); }

void MainWindow::search() {
    ActivitySearchDialog dialog(activityList, this);
    dialog.exec();
}

void MainWindow::about() { QMessageBox::about(this, "Task Organizer Pro", "Applicazione completata con form dinamico, polimorfo e menu di sistema ripristinati."); }
