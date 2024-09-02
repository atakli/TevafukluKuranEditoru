#ifndef QURANNOTEPAD_H
#define QURANNOTEPAD_H

#include <QMainWindow>
#include <QFile>

QT_BEGIN_NAMESPACE
namespace Ui {
class Notepad;
}
QT_END_NAMESPACE

class QuranNotepad : public QMainWindow
{
    Q_OBJECT

    // TODO: tevafuklari otomatik bulacak ve gosterecek. yeni duzenleme ile birlikte tevafuk olusursa bildirim ile, ve oncekileri de uzerine gelince.

public:
    explicit QuranNotepad(QWidget *parent = nullptr);
    ~QuranNotepad();

private slots:
    void newDocument();
    void open();
    void save();
    void saveAs();
    void selectFont();
    void setFontUnderline(bool underline);
    void about();

    void on_gotopage_button_clicked();
    void on_nextPage_clicked();
    void on_prevPage_clicked();
    void on_gotopage_spinBox_editingFinished();

private:
    Ui::Notepad *ui;
    QString currentFile;

    QFont current_font = QFont("Hüsrev Hattı Arapça", 36);
    int previous_page = 3;
    int current_page = 3;

    QFile quran, pages;

    QStringList pages_lines, quran_lines;

    void setLineNumber(int number);
    void set_ayas(const QString& sura, int next_sura, int aya, int next_aya);
    void openPage(int page);    // std::pair<int, int>
    void justifyText();
    void expandTextToLines(int targetLines);
    bool applySavedArrangement(int page);
    bool controlCurrentPageNumber();
};

#endif // QURANNOTEPAD_H
