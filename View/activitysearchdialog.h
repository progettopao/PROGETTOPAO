#ifndef ACTIVITYSEARCHDIALOG_H
#define ACTIVITYSEARCHDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QTableWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QVector>
#include "abstract_activity.h"

class MainWindow;

class ActivitySearchDialog : public QDialog {
    Q_OBJECT

private:
    MainWindow *mainWindow;
    const QVector<Abstract_Activity*>& allActivities; // Riferimento al vettore originale della MainWindow
    QVector<Abstract_Activity*> filteredActivities;   // Vettore locale per tracciare solo i risultati filtrati

    // Elementi grafici
    QLineEdit *searchLineEdit;
    QTableWidget *resultsTable;
    QPushButton *viewDetailsButton;
    QPushButton *closeButton;

    void setupUI();
    void setupConnections();
    void populateTable();

private slots:
    void handleSearch(const QString &text); // Scatta ogni volta che l'utente digita una lettera
    void handleViewDetails();               // Scatta quando si vuole vedere l'elemento selezionato

public:
    ActivitySearchDialog(const QVector<Abstract_Activity*>& activities, MainWindow *parent);
    ~ActivitySearchDialog() = default;
};

#endif // ACTIVITYSEARCHDIALOG_H
