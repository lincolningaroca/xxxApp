#include "acercadedialog.hpp"
#include "ui_acercadedialog.h"
#include "util/helper.hpp"

#include <QMessageBox>

AcercaDeDialog::AcercaDeDialog(const QString& colorMode, QWidget *parent) :
  QDialog(parent), ui(new Ui::AcercaDeDialog){
  ui->setupUi(this);

  setWindowFlags(windowFlags() | Qt::MSWindowsFixedSizeDialogHint);

  ui->tabWidget->setCurrentIndex(0);

  (colorMode == QStringLiteral("Modo Oscuro")) ? ui->lblLogo->setPixmap(QPixmap(":/img/logoEmpresa.png").scaled(490, 338)) :
                                                 ui->lblLogo->setPixmap(QPixmap(":/img/logoEmpresa_1.png").scaled(490, 338));


  loadInfo_app();
  ui->lblLicencia->setText(QStringLiteral("<a href='message'>Ver licencia.</a>"));
  QObject::connect(ui->btnCerrar, &QPushButton::clicked, this, &AcercaDeDialog::reject);

  QObject::connect(ui->lblLicencia, &QLabel::linkActivated, this, [&](){
      QDialog licenciaDlg(this);

      QScopedPointer<QVBoxLayout> mainLayOut(new QVBoxLayout(&licenciaDlg));
      QScopedPointer<QTextBrowser> teLicencia(new QTextBrowser(&licenciaDlg));

      QFile fileName(":/licencia/licencia.txt");
      if (!fileName.open(QFile::ReadOnly | QFile::Text)) {
          QMessageBox::warning(nullptr, SW::Helper_t::appName(), "Error al abrir el archivo.\n" +
                               fileName.errorString());
          return;
        }

      teLicencia->setAcceptRichText(true);
      teLicencia->setOpenExternalLinks(true);
      teLicencia->setHtml(fileName.readAll());
      teLicencia->setReadOnly(true);
      mainLayOut->addWidget(teLicencia.data());
      licenciaDlg.setLayout(mainLayOut.data());
      licenciaDlg.setFixedSize(600, 477);
      licenciaDlg.setWindowTitle(SW::Helper_t::appName().append(" - licencia"));
      licenciaDlg.exec();
    });
}

AcercaDeDialog::~AcercaDeDialog()
{
  delete ui;
}

void AcercaDeDialog::loadInfo_app() const noexcept{
  ui->tbLicencia->setAcceptRichText(true);
  ui->tbLicencia->setOpenExternalLinks(true);
  ui->tbLicencia->setHtml(
        "<p><font size=\"4\">xxxApp:<br><br>Es software libre, puede "
        "redistribuirlo y/o modificarlo bajo los términos de la Licencia Pública "
        "General de GNU según se encuentra publicada por la <a "
        "href=\"https://www.fsf.org\">Free Software "
        "Foundation</a>, bien de la versión 3 de dicha Licencia o bien (según su "
        "elección) de cualquier versión posterior.<br><br>"
        "Este programa se distribuye con la esperanza de que sea útil, pero <strong>SIN "
        "NINGUNA "
        "GARANTÍA</strong>, incluso sin la garantía <strong>MERCANTIL</strong> implícita ni la de "
        "garantizar la <strong>ADECUACIÓN A UN PROPÓSITO PARTICULAR.</strong> Véase la <a "
        "href=\"https://www.gnu.org/licenses/\">Licencia "
        "Pública General</a> de GNU para más detalles.</font></p>");

}
