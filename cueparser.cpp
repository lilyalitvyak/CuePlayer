#include "cueparser.h"

enum Codec {
	AUTO,
	CP1250,
	CP1251,
	UTF8
};

CueParser::CueParser(QString s, int codec)
{
	int in = 0;
	trackNumber = 0;
	QRegExp rxPerformer("^PERFORMER \"(.*)\"");
	QRegExp rxSoundfile("^FILE \"(.*)\"");
	QRegExp rxTitle("TITLE \"(.*)\"");
	QRegExp rxIndex("INDEX \\d+ (.*)");
	QRegExp rxTrackNumber("TRACK (\\d\\d) .*");
	QRegExp rxAudioPath("(.*/).*");
	QRegExp rxFileWav("(.*)(\\.wav)");
        QRegExp rxTagCue("^cuesheet=.*");

	rxFileWav.setCaseSensitivity(Qt::CaseInsensitive);
	rxAudioPath.indexIn(s);
	audioPath = rxAudioPath.cap(1);

        QTextStream cuetext;
        QFile cuefile;
        if (rxTagCue.indexIn(s) != -1)
        {
            s.remove(0,9);
            cuetext.setString(&s);
        }
        else
        {
            cuefile.setFileName(s);
            if (!cuefile.open(QIODevice::ReadOnly | QIODevice::Text))
                    return;
            cuetext.setDevice(&cuefile);
        }

	switch (codec) {
	case AUTO:
		cuetext.setCodec("Windows-1251");
		cuetext.setAutoDetectUnicode(true);
		break;
	case CP1250:
		cuetext.setCodec("Windows-1250");
		break;
	case CP1251:
		cuetext.setCodec("Windows-1251");
		break;
	case UTF8:
		cuetext.setCodec("UTF-8");
		break;
	default:
		cuetext.setCodec("Windows-1251");
		cuetext.setAutoDetectUnicode(true);
		break;
	}

	do {
		line = cuetext.readLine();
		if (rxPerformer.indexIn(line) != -1)
			parsedFile.performer = rxPerformer.cap(1);
		else if (rxSoundfile.indexIn(line) != -1)
			parsedFile.soundfile = rxSoundfile.cap(1);
		else if (rxTitle.indexIn(line) != -1)
		{
			if (trackNumber)
				tracks[++in].title = rxTitle.cap(1);
			else
				parsedFile.album = rxTitle.cap(1);
		}
		else if (rxIndex.indexIn(line) != -1)
		{
			tracks[in].index = rxIndex.cap(1);
			tracks[in].file = parsedFile.soundfile;
		}
		else if (rxTrackNumber.indexIn(line) != -1)
			trackNumber = rxTrackNumber.cap(1).toInt(0,10);

		if (in == 99)
			break;
	} while (!line.isNull());

	if (!in)
	{
		cuetext.seek(0);
		if(!cuetext.pos())
		{
			do {
				line = cuetext.readLine();
				if (rxSoundfile.indexIn(line) != -1)
					parsedFile.soundfile = rxSoundfile.cap(1);
				else if (rxTrackNumber.indexIn(line) != -1)
					++in;
				else if (rxIndex.indexIn(line) != -1)
				{
					tracks[in].index = rxIndex.cap(1);
					tracks[in].title = parsedFile.soundfile;
					tracks[in].file = parsedFile.soundfile;
				}

				if (in == 99)
					break;
			} while (!line.isNull());
		}
		/*else
			QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Ошибка чтения CUE файла."));*/
	}
	cuefile.close();

	if (rxFileWav.indexIn(parsedFile.soundfile) != -1)
	{
		QString soundfile = audioPath + "/" + rxFileWav.cap(1);
		QFile sndfh;
		if (!sndfh.exists(soundfile + rxFileWav.cap(2)))
		{
			if (!sndfh.exists(soundfile + ".ape"))
				if (!sndfh.exists(soundfile + ".flac"))
					;//QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Медиафайл не найден."));
				else
					parsedFile.soundfile = rxFileWav.cap(1) + ".flac";
			else
				parsedFile.soundfile = rxFileWav.cap(1) + ".ape";
		}
		sndfh.close();
	}
	for (int i = in; i > 0; i--)
	{
		if (rxFileWav.indexIn(tracks[i].file) != -1)
		{
			QString soundfile = audioPath + "/" + rxFileWav.cap(1);
			QFile sndfh;
			if (!sndfh.exists(soundfile + rxFileWav.cap(2)))
			{
				if (!sndfh.exists(soundfile + ".ape"))
					if (!sndfh.exists(soundfile + ".flac"))
						;//QMessageBox::critical(this, trUtf8("Ошибка"), trUtf8("Медиафайл не найден."));
					else
						tracks[i].file = rxFileWav.cap(1) + ".flac";
				else
					tracks[i].file = rxFileWav.cap(1) + ".ape";
			}
			sndfh.close();
		}
	}
}

QString CueParser::getTitle()
{
	return parsedFile.performer + " - " + parsedFile.album;
}

QString CueParser::getPerformer()
{
	return parsedFile.performer;
}

QString CueParser::getAlbum()
{
	return parsedFile.album;
}

QString CueParser::getTrackTitle(int ind)
{
	return tracks[ind].title;
}

qint64 CueParser::getTrackIndex(int ind)
{
	QRegExp rxIndexMil("(\\d\\d):(\\d\\d):(\\d\\d)");
	rxIndexMil.indexIn(tracks[ind].index);
	indexMil = rxIndexMil.cap(1).toInt(0,10) * 60000 +
	rxIndexMil.cap(2).toInt(0,10) * 1000 +
        rxIndexMil.cap(3).toInt(0,10) * 13;
	return indexMil;
}

QString CueParser::getSoundFile()
{
	return audioPath + parsedFile.soundfile;
}

int CueParser::getTrackNumber()
{
	return trackNumber;
}

QString CueParser::getTrackFile(int ind)
{
	return audioPath + tracks[ind].file;
}
