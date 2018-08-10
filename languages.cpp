#include "languages.h"

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QDebug>

#include "preferences.h"
#include "shubetriaapp.h"

Languages::Languages()
{
}

//QStringList Languages::getSupportedLanguagesList(void)
//{

//    QStringList supportedLanguages;
//    Languages lang;
//    supportedLanguages.clear();

//    QDirIterator translationFiles(":/language"
//                                  , QStringList("Shubetria*.qm")
//                                  , QDir::Files
//                                  , QDirIterator::Subdirectories);

//    while (translationFiles.hasNext())
//    {
//        translationFiles.next();

//        QString file = translationFiles.fileName();
//        supportedLanguages.append(lang.convertFileToLanguage(file));
//    }

//    return supportedLanguages;
//}

//QString Languages::convertLanguageToFile(QString lang)
//{
//    QString language_code;

//    if (lang == "German")
//    {
//        language_code = "de";
//    } else if (lang == "English") {
//        language_code = "en";
//    } else if (lang == "Italian") {
//        language_code = "it";
//    }

//    QString file = QString(":/language/shubetria_%1.qm").arg(language_code);

//    return file;
//}

//QString Languages::convertFileToLanguage(QString file)
//{
//    file.truncate(file.lastIndexOf('.'));
//    file.remove(0, file.lastIndexOf('_') +1);

//    QString lang = QLocale::languageToString(QLocale(file).language());

//    return lang;
//}
