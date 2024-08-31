#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#if defined(QT_PRINTSUPPORT_LIB)
#include <QtPrintSupport/qtprintsupportglobal.h>
#if QT_CONFIG(printer)
#if QT_CONFIG(printdialog)
#include <QPrintDialog>
#endif // QT_CONFIG(printdialog)
#include <QPrinter>
#endif // QT_CONFIG(printer)
#endif // QT_PRINTSUPPORT_LIB
#include <QFont>
#include <QFontDatabase>
#include <QFontDialog>
#include <QSize>

#include "notepad.h"
#include "ui_notepad.h"

Notepad::Notepad(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Notepad)
{
    ui->setupUi(this);

    connect(ui->actionNew, &QAction::triggered, this, &Notepad::newDocument);
    connect(ui->actionOpen, &QAction::triggered, this, &Notepad::open);
    connect(ui->actionSave, &QAction::triggered, this, &Notepad::save);
    connect(ui->actionSave_as, &QAction::triggered, this, &Notepad::saveAs);
    connect(ui->actionPrint, &QAction::triggered, this, &Notepad::print);
    connect(ui->actionExit, &QAction::triggered, this, &QWidget::close);
#if QT_CONFIG(clipboard)
    connect(ui->textEdit, &QTextEdit::copyAvailable, ui->actionCopy, &QAction::setEnabled);
    connect(ui->actionCopy, &QAction::triggered, ui->textEdit, &QTextEdit::copy);
    connect(ui->actionCut, &QAction::triggered, ui->textEdit, &QTextEdit::cut);
    connect(ui->actionPaste, &QAction::triggered, ui->textEdit, &QTextEdit::paste);
#endif
    connect(ui->textEdit, &QTextEdit::undoAvailable, ui->actionUndo, &QAction::setEnabled);
    connect(ui->actionUndo, &QAction::triggered, ui->textEdit, &QTextEdit::undo);
    connect(ui->textEdit, &QTextEdit::redoAvailable, ui->actionRedo, &QAction::setEnabled);
    connect(ui->actionRedo, &QAction::triggered, ui->textEdit, &QTextEdit::redo);

    connect(ui->actionFont, &QAction::triggered, this, &Notepad::selectFont);
    connect(ui->actionBold, &QAction::triggered, this, &Notepad::setFontBold);
    connect(ui->actionUnderline, &QAction::triggered, this, &Notepad::setFontUnderline);
    connect(ui->actionItalic, &QAction::triggered, this, &Notepad::setFontItalic);
    connect(ui->actionAbout, &QAction::triggered, this, &Notepad::about);

    quran.setFileName("quran/quran-simple-clean-besmelesiz.txt");
    if (!quran.open(QFileDevice::ReadOnly))
    {
        qCritical() << "cannot open quran file: " << quran.errorString();
    }
    pages.setFileName("quran/pages.txt");
    if (!pages.open(QFileDevice::ReadOnly))
    {
        qCritical() << "cannot open pages file: " << pages.errorString();;
    }
    QTextStream pages_in(&pages);
    while (!pages_in.atEnd())
    {
        const QString line = pages_in.readLine();
        pages_lines.append(line);
    }

    QTextStream quran_in(&pages);
    while (!pages_in.atEnd())
    {
        const QString line = pages_in.readLine();
        quran_lines.append(line);
    }

    openPage(current_page);

    // const QStringList arr = QFontDatabase::families(QFontDatabase::WritingSystem::Arabic);
    // for (const auto& a : arr)
    //     qDebug() << a;

    ui->textEdit->setFont(current_font);
    setLineNumber(15);                                   // TODO: config dosyasindan al veya user-defined bir deger yap

// Disable menu actions for unavailable features
#if !defined(QT_PRINTSUPPORT_LIB) || !QT_CONFIG(printer)
    ui->actionPrint->setEnabled(false);
#endif

#if !QT_CONFIG(clipboard)
    ui->actionCut->setEnabled(false);
    ui->actionCopy->setEnabled(false);
    ui->actionPaste->setEnabled(false);
#endif
}

Notepad::~Notepad()
{
    delete ui;
}

void Notepad::setLineNumber(int number)
{
    ui->listWidget->clear();
    for (int i = 1; i <= number; ++i)
    {
        ui->listWidget->addItem(QString::number(i));
    }
    ui->listWidget->setFont({ui->listWidget->font().family(), ui->textEdit->font().pointSize()});
    // ui->listWidget->setFixedWidth(25);                  // TODO: generic biseyle degistir. edit: evet generic biseyle degistir ama simdilik burdan alip ui'a koydum.
}

void Notepad::set_ayas(const QString& sura, int next_sura, int aya, int next_aya)
{
    QFile suraFile("quran/suras/" + sura + ".txt");
    if (!suraFile.open(QFileDevice::ReadOnly))
    {
        qCritical() << "cannot open sura file: " << suraFile.errorString();
    }
    QTextStream lines_in(&suraFile);
    QStringList ayas;
    int current_line = 1;
    while (!lines_in.atEnd())
    {
        const QString line = lines_in.readLine();
        // for (int i = )
        if (aya <= current_line && current_line < next_aya)
            ayas.append(line);
        current_line++;
    }

    // for (int i = sura.toInt(); i <= next_sura; ++i)
    // {
    //     QTextStream lines_in(&suraFile);
    //     QStringList ayas;
    //     int current_line = 1;
    //     while (!lines_in.atEnd())
    //     {
    //         const QString line = lines_in.readLine();
    //         // for (int i = )
    //         if (aya <= current_line && current_line < next_aya)
    //             ayas.append(line);
    //         current_line++;
    //     }
    // }
    ui->textEdit->setText(ayas.join(" "));
    ui->gotopage_spinBox->setValue(current_page);

    expandTextToLines(15);
    // justifyText();

    QTextDocument *doc = ui->textEdit->document();
    qDebug() << "lnes:" << doc->blockCount();
}
void Notepad::justifyText()
{
    // ui->textEdit->setCurrentCharFormat(Qt::RichText);

    // QTextCursor cursor = ui->textEdit->textCursor();
    // QTextBlockFormat blockFormat;
    // blockFormat.setAlignment(Qt::AlignJustify);
    // cursor.select(QTextCursor::Document);
    // cursor.mergeBlockFormat(blockFormat);
    // ui->textEdit->setTextCursor(cursor);
    QString currentText = ui->textEdit->toPlainText();
    QString justifiedText = "<div align='justify'>" + currentText.toHtmlEscaped().replace("\n", "<br>") + "</div>";
    ui->textEdit->setHtml(justifiedText);
}
void Notepad::expandTextToLines(int targetLines)
{
    QTextDocument *doc = ui->textEdit->document();
    QString text = doc->toPlainText();

    // Split the text into words
    QStringList words = text.split(' ', Qt::SkipEmptyParts);

    int currentLines = doc->blockCount();
    if (currentLines >= targetLines) {
        return;  // No need to expand if the text already has enough lines
    }

    // Calculate the number of words per line
    int totalWords = words.size();
    int wordsPerLine = totalWords / targetLines;
    int extraWords = totalWords % targetLines;

    QStringList expandedLines;
    int wordIndex = 0;

    // Distribute words across lines
    for (int i = 0; i < targetLines; ++i) {
        int currentLineWords = wordsPerLine + (i < extraWords ? 1 : 0);
        QStringList lineWords = words.mid(wordIndex, currentLineWords);
        expandedLines.append(lineWords.join(' '));
        wordIndex += currentLineWords;
    }

    // Join the expanded lines back together with line breaks
    ui->textEdit->setPlainText(expandedLines.join('\n'));
}
void Notepad::openPage(int page)    // simdilik fatiha ile bakaranin ilk sayfasini ayni sayfada gibi gostericem
{
    const QStringList sura_and_aya = pages_lines[page - 1].split(" ");   // if the page is 1, the index should be 0.
    const QStringList next_sura_and_aya = pages_lines[page].split(" ");
    const QString sura = sura_and_aya[0];               // 2
    const int aya = sura_and_aya[1].toInt();            // 17
    const int next_sura = next_sura_and_aya[0].toInt(); // 2
    const int next_aya = next_sura_and_aya[1].toInt();  // 25

    set_ayas(sura, next_aya, aya, next_aya);
}

void Notepad::newDocument()
{
    currentFile.clear();
    ui->textEdit->setText(QString());
}

void Notepad::open()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open the file");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    currentFile = fileName;
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream in(&file);
    QString text = in.readAll();
    ui->textEdit->setText(text);
    file.close();
}

void Notepad::save()
{
    QString fileName;
    // If we don't have a filename from before, get one.
    if (currentFile.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save");
        if (fileName.isEmpty())
            return;
        currentFile = fileName;
    } else {
        fileName = currentFile;
    }
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void Notepad::saveAs()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save as");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    currentFile = fileName;
    setWindowTitle(fileName);
    QTextStream out(&file);
    QString text = ui->textEdit->toPlainText();
    out << text;
    file.close();
}

void Notepad::print()
{
#if defined(QT_PRINTSUPPORT_LIB) && QT_CONFIG(printer)
    QPrinter printDev;
#if QT_CONFIG(printdialog)
    QPrintDialog dialog(&printDev, this);
    if (dialog.exec() == QDialog::Rejected)
        return;
#endif // QT_CONFIG(printdialog)
    ui->textEdit->print(&printDev);
#endif // QT_CONFIG(printer)
}

void Notepad::selectFont()
{
    bool fontSelected;
    current_font = QFontDialog::getFont(&fontSelected, current_font, this);
    if (fontSelected)
    {
        ui->textEdit->setFont(current_font);
        setLineNumber(15);
    }
}

void Notepad::setFontUnderline(bool underline)
{
    ui->textEdit->setFontUnderline(underline);
}

void Notepad::setFontItalic(bool italic)
{
    ui->textEdit->setFontItalic(italic);
}

void Notepad::setFontBold(bool bold)
{
    bold ? ui->textEdit->setFontWeight(QFont::Bold) :
           ui->textEdit->setFontWeight(QFont::Normal);
}

void Notepad::about()
{
    QMessageBox::about(this, tr("About Notepad"),
                       tr("The <b>Notepad</b> example demonstrates how to code a basic "
                          "text editor using QtWidgets"));
}

void Notepad::on_gotopage_button_clicked()
{
    current_page = ui->gotopage_spinBox->text().toInt();
    openPage(current_page);
}


void Notepad::on_nextPage_clicked()
{
    if (current_page == 604)
    {
        return;                     // TODO: bu zaten son sayfa diye bir uyari konabilir
    }
    openPage(++current_page);
}


void Notepad::on_prevPage_clicked()
{
    if (current_page == 1)
    {
        return;
    }
    openPage(--current_page);       // TODO: bu zaten ilk sayfa diye bir uyari konabilir
}


void Notepad::on_gotopage_spinBox_editingFinished()
{
    current_page = ui->gotopage_spinBox->value();
    openPage(current_page);
}

