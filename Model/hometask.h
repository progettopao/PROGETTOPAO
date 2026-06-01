#ifndef HOMETASK_H
#define HOMETASK_H

#include "abstract_activity.h"

class HomeTask : public Abstract_Activity {
private:
    QString stanza;
    int urgenza;

public:
    // Costruttore
    HomeTask(const QString &id, const QString &titolo, const QString &descrizione,
             bool completata, const QString &stanza, int urgenza);

    // Getters
    QString getStanza() const;
    int getUrgenza() const;

    // Setters
    void setStanza(const QString &stanza);
    void setUrgenza(int urgenza);

    // Override dei metodi virtuali puri della classe base
    QString getDettagliSpecifici() const override;
    bool isUrgente() const override;

    // Override e metodi per la persistenza dati
    QJsonObject toJsonObject() const override;
    Abstract_Activity* cloneFromJson(const QJsonObject &json) const override;
    void writeToXml(QXmlStreamWriter &writer) const override;
};

#endif // HOMETASK_H
