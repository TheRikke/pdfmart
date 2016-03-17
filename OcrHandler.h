#ifndef OCR_HANDLER_H
#define OCR_HANDLER_H

#include <QProcess>

class OcrHandler {
public:
   OcrHandler();

   QString AddTextToPDF(const QString &pdfInputName, const QString &pdfOutputName);
   bool IsReady() const;

private:
   bool IsTesseractReady;
   QProcess TesseractProcess;
};

#endif //OCR_HANDLER_H
