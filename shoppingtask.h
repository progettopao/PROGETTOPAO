#ifndef SHOPPINGTASK_H
#define SHOPPINGTASK_H

#include "abstract_activity.h"
#include <QStringList>

class ShoppingTask : public Abstract_Activity {
private:
    QString supermercatoConsigliato;
    double budgetMassimo;
    QStringList listaProdotti;

public:
    ShoppingTask(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                 const QString& negozio, double budget);

    // Getter e Setter
    QString getSupermercatoConsigliato() const;
    void setSupermercatoConsigliato(const QString& negozio);
    double getBudgetMassimo() const;
    void setBudgetMassimo(double budget);
    QStringList getListaProdotti() const;

    void aggiungiArticolo(const QString& articolo);
    void spuntaArticolo(const QString& articolo);

    // Override Polimorfismo
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;
    void completaAttivita();

    // Override Persistenza
    QJsonObject toJsonObject() const override;
    void fromJsonObject(const QJsonObject& json) override;
    void writeToXml(QXmlStreamWriter& writer) const override;
};

#endif // SHOPPINGTASK_H
