#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QDateEdit>
#include <QVector>
#include <QToolBar>
#include "abstract_activity.h"
#include "jsonpersistencemanager.h"
#include "xmlpersistencemanager.h"

class ActivityListWidget;
class ActivityDetailWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QVector<Abstract_Activity*>& getActivityList();
    void refreshView();

    // RISOLUZIONE ERRORE: Spostati in un blocco pubblico in modo che widget esterni
    // come ActivityListWidget e ActivitySearchDialog possano invocarli liberamente.
public slots:
    void addActivity();
    void editActivity(Abstract_Activity *activity);
    void showActivityDetails(Abstract_Activity *activity);
    void showActivityList();
    void removeActivity(Abstract_Activity *activity);
    void search();

    void newLibrary();
    void openLibrary();
    void saveLibrary();
    void saveLibraryAs();
    void saveFormUnder();
    void about();

private:
    void setupUI();
    void setupMenus();
    void setupConnections();
    void updateWindowTitle();
    void clearActivities();

    QVector<Abstract_Activity*> activityList;
    QString currentFilePath;
    Abstract_Activity *activityUnderEdit;
    QString currentNewType;

    JsonPersistenceManager jsonPersistence;
    XmlPersistenceManager xmlPersistence;

    // Componenti UI della parte superiore
    QStackedWidget *centralStack;
    QToolBar *toolbar;
    ActivityListWidget *listWidget;
    ActivityDetailWidget *detailWidget;

    // Elementi del Form inferiore
    QWidget *formContainer;
    QFormLayout *formLayout;
    QComboBox *cmbCategoria;
    QLineEdit *txtTitolo;
    QLineEdit *txtDescrizione;
    QCheckBox *chkCompletata;

    QStackedWidget *formTypeStack;

    // Campi specifici delle sottoclassi
    QDoubleSpinBox *dspImporto;
    QLineEdit *txtUtenza;
    QDateEdit *datScadenza;

    QLineEdit *txtStanza;
    QSpinBox *spnUrgenza;

    QLineEdit *txtVeicolo;
    QLineEdit *txtTipoIntervento;
    QLineEdit *txtComponenti;

    QLineEdit *txtNegozio;
    QDoubleSpinBox *dspBudget;
    QLineEdit *txtProdotti;

    QLineEdit *txtTipoSvago;
    QSpinBox *spnPriorita;
    QLineEdit *txtCompagnia;
};

#endif // MAINWINDOW_H
