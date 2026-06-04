#ifndef ACTIVITYLISTWIDGET_H
#define ACTIVITYLISTWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>

#include "abstract_activity.h"

// Pre-dichiarazione della MainWindow per evitare inclusioni circolari negli header
class MainWindow;

class ActivityListWidget : public QWidget {
    Q_OBJECT

private:
    MainWindow *mainWindow;       // Puntatore alla finestra principale per accedere al vettore dati
    QTableWidget *tableWidget;    // Tabella per mostrare le attività (ID, Titolo, Tipo, Stato)

    QComboBox *viewModeCombo;     // Menu a tendina per filtri di visualizzazione (es. "Tutte", "Solo Urgenti")
    QComboBox *filterTypeCombo;   // Menu a tendina per filtrare per classe (es. "Bill", "HomeTask")
    QPushButton *addButton;       // Pulsante rapido per aggiungere un'attività

    void setupUI();
    void setupConnections();
    void setupTableHeaders();     // Configura le colonne della tabella

public:
    ActivityListWidget(MainWindow *mainWindow);

    // Metodo fondamentale che ridisegna la tabella leggendo i dati aggiornati dalla MainWindow
    void updateActivityList();

private slots:
    void handleSelection();
    void handleCellDoubleClick(int row, int column);
    void changeViewMode(int index);
    void filterByType(int index);

signals:
    // Segnali emessi verso la MainWindow per coordinare la navigazione e il CRUD
    void activitySelected(Abstract_Activity *activity);
    void editRequested(Abstract_Activity *activity);
    void removeRequested(Abstract_Activity *activity);
};

#endif // ACTIVITYLISTWIDGET_H
