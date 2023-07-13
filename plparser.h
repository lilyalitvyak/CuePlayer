#ifndef PLPARSER_H
#define PLPARSER_H

#include <QStringList>
#include <QNetworkReply>
#include <QFileInfo>
#include <QTextStream>
#include <QXmlStreamReader>

class PlParser : public QObject
{
	Q_OBJECT

public:
	PlParser(QObject *parent = 0);
	void setPlUri(QString);
	unsigned short getPlEntries();
	QStringList getPlStruct(unsigned short);
private:
	QNetworkAccessManager *manager;
	unsigned short entries;
	struct ParsedPls {
		QString uri;
		QString title;
	};
	ParsedPls playlist[100];
	void parsePls(QStringList);
	void parseWvx(QStringList);
        void parseM3u(QStringList,QString);
private slots:
	void readNmReply(QNetworkReply*);
signals:
	void plperror(QString);
	void ready();
};

#endif // PLPARSER_H
