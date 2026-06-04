#include "activitydetailwidget.h"
#include <QFormLayout>

ActivityDetailWidget::ActivityDetailWidget(QWidget *parent)
    : QWidget(parent), currentActivity(nullptr) {

    setupUI();
    setupConnections();
}

void ActivityDetailWidget::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // BARRA SUPERIORE (Navigazione e Azioni)
    QHBoxLayout *topBarLayout = new QHBoxLayout();

    backButton = new QPushButton("Torna alla Lista", this);
    editButton = new QPushButton("Modifica", this);
    deleteButton = new QPushButton("Elimina", this);

    topBarLayout->addWidget(backButton);
    topBarLayout->addStretch(); // Spinge il tasto modifica e elimina sulla destra
    topBarLayout->addWidget(editButton);
    topBarLayout->addWidget(deleteButton);

    mainLayout->addLayout(topBarLayout);

    // AREA CENTRALE
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);

    contentWidget = new QWidget();
    QVBoxLayout *contentLayout = new QVBoxLayout(contentWidget);

    // Titolo dell'attività
    titleLabel = new QLabel(this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #2c3e50; margin-bottom: 15px;");
    titleLabel->setWordWrap(true);
    contentLayout->addWidget(titleLabel);

    // Gruppo Informazioni Generali
    QLabel *lblSezioneGen = new QLabel("Informazioni Generali", this);
    lblSezioneGen->setStyleSheet("font-size: 12px; font-weight: bold; color: #7f8c8d; text-transform: uppercase;");
    contentLayout->addWidget(lblSezioneGen);

    generalDetailsLabel = new QLabel(this);
    generalDetailsLabel->setStyleSheet("font-size: 14px; padding: 10px; background-color: #f8f9fa; border-radius: 4px; border-left: 4px solid #3498db;");
    generalDetailsLabel->setWordWrap(true);
    contentLayout->addWidget(generalDetailsLabel);

    contentLayout->addSpacing(15);

    // Gruppo Specifico della Sottoclasse
    QLabel *lblSezioneSpec = new QLabel("Dettagli Specifici Categoria", this);
    lblSezioneSpec->setStyleSheet("font-size: 12px; font-weight: bold; color: #7f8c8d; text-transform: uppercase;");
    contentLayout->addWidget(lblSezioneSpec);

    specificDetailsLabel = new QLabel(this);
    specificDetailsLabel->setStyleSheet("font-size: 14px; padding: 10px; background-color: #f4f6f7; border-radius: 4px; border-left: 4px solid #2ecc71; font-family: monospace;");
    specificDetailsLabel->setWordWrap(true);
    contentLayout->addWidget(specificDetailsLabel);

    contentLayout->addStretch();

    scrollArea->setWidget(contentWidget);
    mainLayout->addWidget(scrollArea);
}

void ActivityDetailWidget::setupConnections() {
    connect(backButton, &QPushButton::clicked, this, &ActivityDetailWidget::backRequested);

    connect(editButton, &QPushButton::clicked, this, [this]() {
        if (currentActivity) {
            emit editRequested(currentActivity);
        }
    });

    connect(deleteButton, &QPushButton::clicked, this, [this]() {
        if (currentActivity) {
            emit deleteRequested(currentActivity);
        }
    });
}

void ActivityDetailWidget::setActivity(Abstract_Activity *activity) {
    currentActivity = activity;
    updateContent();
}

void ActivityDetailWidget::updateContent() {
    if (!currentActivity) {
        titleLabel->setText("Nessuna attività selezionata.");
        generalDetailsLabel->clear();
        specificDetailsLabel->clear();
        return;
    }

    // 1. Popola il titolo
    titleLabel->setText(currentActivity->getTitolo());

    // 2. Popola i dettagli generali (comuni a tutte le attività)
    QString statoTesto = currentActivity->isCompletata() ? "Completata" : "In Corso";
    QString urgenzaTesto = currentActivity->isUrgente() ? "Si" : "No";

    QString infoGenerali = QString(
                               "<b>Descrizione:</b> %1<br><br>"
                               "<b>Stato Corrente:</b> %2<br>"
                               "<b>Urgenza Assegnata:</b> %3"
                               ).arg(currentActivity->getDescrizione()).arg(statoTesto).arg(urgenzaTesto);

    generalDetailsLabel->setText(infoGenerali);

    QString dettagliSpecifici = currentActivity->getDettagliSpecifici();

    if (dettagliSpecifici.isEmpty()) {
        specificDetailsLabel->setText("Nessun dettaglio aggiuntivo disponibile per questa categoria.");
    } else {
        specificDetailsLabel->setText(dettagliSpecifici);
    }
}
