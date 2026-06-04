#ifndef ACTIVITYDETAILWIDGET_H
#define ACTIVITYDETAILWIDGET_H

#include <QWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>

#include "abstract_activity.h"

class ActivityDetailWidget : public QWidget {
    Q_OBJECT

private:
    QScrollArea *scrollArea;
    QWidget *contentWidget;

    // Elementi grafici per mostrare i dati dell'attività
    QLabel *titleLabel;             // Mostrerà il Titolo in grande
    QLabel *generalDetailsLabel;    // Mostrerà Descrizione e Stato (Completata/In Corso)
    QLabel *specificDetailsLabel;   // Mostrerà i dettagli polimorfi (es. Importo, Ente per Bill)

    QPushButton *backButton;
    QPushButton *editButton;
    QPushButton *deleteButton;

    Abstract_Activity *currentActivity;

    void setupUI();
    void setupConnections();
    void updateContent(); // Si occuperà di aggiornare i testi quando cambia l'attività

public:
    ActivityDetailWidget(QWidget *parent = nullptr);

    // Metodo fondamentale chiamato dalla MainWindow per passare l'oggetto da mostrare
    void setActivity(Abstract_Activity *activity);

signals:
    void backRequested();
    void editRequested(Abstract_Activity *activity);
    void deleteRequested(Abstract_Activity *activity);
};

#endif // ACTIVITYDETAILWIDGET_H
