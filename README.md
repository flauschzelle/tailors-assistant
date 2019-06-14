# SoSe19_Padi20

### Student
Kirstin Rohwer, Bachelor Informatik

### Projektname/working title
Tailor's Assistant

### Kurze Zusammenfassung
Ein Tool, das bei der Abschätzung von Arbeitszeiten für die Preiskalkulation in der Maßschneiderei hilft. Es berechnet Zeit-Schätzungen für einzelne Arbeitsschritte anhand von Werten aus vergangenen Aufträgen, die in einer Datenbank gespeichert werden.

### Detaillierte Beschreibung/Auflistung der Funktionalität
- Grafische Oberfläche mit Möglichkeiten zur Eingabe und zur Abfrage von Daten
- Eingabe: Werte aus vergangenen Aufträgen erfassen
	- Stammdaten für jedes Werkstück: Datum, Kunde, Bezeichnung des Werkstücks, Typ, *(optional: Materialien, Foto)*
	- Arbeitsschritte mit Bezeichnung, Zeit, Anzahl, *(optional: Nahttyp, Material, Detail, Kommentar)*
	- Möglichkeit, gespeicherte Daten nachträglich zu bearbeiten oder zu löschen
- Abfrage: neues Werkstück kalkulieren
	- *optional: Stammdaten eingeben*
	- Typ auswählen
	- *optional: anhand des Typs werden Arbeitsschritte und/oder Details vorgeschlagen*
	- *optional: Details auswählen, Arbeitschritte werden anhand der Details vorgeschlagen*
	- Arbeitsschritte hinzufügen (Auswahl aus Menü mit allen bisherigen Bezeichnungen + Möglichkeit, neue Schritte zu definieren), Anzahl eingeben
	- *optional: Filtern nach Material, Detail, Nahttyp (pro Schritt einstellbar)*
	- parallel zur Eingabe erscheint daneben eine Liste mit Werten für die gewählten Arbeitsschritte mit Anzahl, Bezeichnung, Statistik (Datenbasis, min, max, med, avg), *(optional: Filterung)*
	- Eingabefeld für manuellen Wert
	- *optional: Möglichkeit, die Reihenfolge der angezeigten Schritte zu ändern*
	- *optional: Klick auf Datenbasis öffnet ein Menü, in dem alle Werkstücke der Datenbasis für diesen Schritt angezeigt werden und einzelne manuell ausgeschlossen werden können*
	- unter der Werteliste eine Zeile mit Summen für die einzelnen Spalten
	- *optional: Möglichkeit, ein so kalkuliertes Angebot als Textdatei zu exportieren*
	- Möglichkeit zum Speichern eines Angebots um es später weiter zu bearbeiten
	- *optional: Möglichkeit, ein gespeichertes Angebot in ein vergangenes Werkstück umzuwandeln und die tatsächlichen Zeiten zu erfassen*

### Welche externen Bibliotheken/Tools werden benutzt und wofür?
(mindestens eine externe Bibliothek muss verwendet werden)
- Qt für grafische Oberfläche
- für Datenbank-Anbindung

### Welche besonderen Anforderungen ergeben sich?
(z.B. komplexe Berechnungen in Echtzeit, 
effiziente Speicherverwaltung/Verarbeitung großer Datenmengen)
