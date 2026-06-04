#ifndef ABSTRACT_ACTIVITY_H
#define ABSTRACT_ACTIVITY_H

#include <QString>
#include <QJsonObject>
#include <QXmlStreamWriter>

class Abstract_Activity {
private:
    QString id;
    QString titolo;
    QString descrizione;
    bool completata;

public:
    // Costruttore (Stringa per ID coerente col .cpp)
    Abstract_Activity(const QString &id, const QString &titolo, const QString &descrizione, bool completata = false);

    // Distruttore virtuale defaultato direttamente qui
    virtual ~Abstract_Activity() = default;

    // Getters
    QString getId() const;
    QString getTitolo() const;
    QString getDescrizione() const;
    bool isCompletata() const;

    // Setters
    void setTitolo(const QString &titolo);
    void setDescrizione(const QString &descrizione);
    void setCompletata(bool stato);
    void completaAttivita();

    // Metodi virtuali e polimorfi
    virtual QString getDettagliSpecifici() const = 0;
    virtual bool isUrgente() const = 0; // Lasciato puro

    // Persistenza
    virtual QJsonObject toJsonObject() const = 0;// Persistenza JSON
    virtual void writeToXml(QXmlStreamWriter &writer) const = 0; // Persistenza XML

    // Il "Clone": Ogni classe concreta implementerà questo metodo per leggere il JSON
    // e restituire una nuova istanza di se stessa, senza usare costrutti di flusso esterni.
    virtual Abstract_Activity* cloneFromJson(const QJsonObject &json) const = 0;
};

#endif // ABSTRACT_ACTIVITY_H
