#ifndef VEHICLEMAINTENANCE_H
#define VEHICLEMAINTENANCE_H

#include "abstract_activity.h"
#include <QStringList>

class VehicleMaintenance : public Abstract_Activity {
private:
    QString targaVeicolo;
    int chilometraggioScadenza;
    QString officinaRiferimento;
    QStringList componentiDaSostituire;

public:
    // Costruttore
    VehicleMaintenance(const QString& id, const QString& titolo, const QString& descrizione, bool completata,
                       const QString& targa, int kmScadenza, const QString& officina);

    // Getter e Setter specifici
    QString getTargaVeicolo() const;
    void setTargaVeicolo(const QString& targa);

    QString getOfficinaRiferimento() const;
    void setOfficinaRiferimento(const QString& officina);

    QStringList getComponentiDaSostituire() const;

    // Gestione della lista dinamica dei componenti
    void aggiungiComponente(const QString& componente);
    void rimuoviComponente(const QString& componente);

    // Override dei metodi polimorfi della classe base
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;

    // Override per la persistenza dati
    QJsonObject toJsonObject() const override;
    void fromJsonObject(const QJsonObject& json) override;
    void writeToXml(QXmlStreamWriter& writer) const override;
};

#endif // VEHICLEMAINTENANCE_H
