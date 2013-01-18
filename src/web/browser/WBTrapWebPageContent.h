#ifndef WB_TRAP_WEB_PAGE_CONTENT_H
#define WB_TRAP_WEB_PAGE_CONTENT_H

#include <QtGui>
#include <QWebView>

#include "WBWebView.h"

class WBTrapWebPageContentWindow : public QDialog
{
public:
    WBTrapWebPageContentWindow(QObject *controller, QWidget *parent = NULL);
    virtual ~WBTrapWebPageContentWindow();

    void setUrl(const QUrl &url);
    void setReadyForTrap(bool bReady);
    QWebView *webView() const {return mTrapContentPreview;}
    QComboBox *itemsComboBox() const {return mSelectContentCombobox;}
    QLineEdit *applicationNameLineEdit() const {return mApplicationNameEdit;}

private:
    QObject *mController;

    QHBoxLayout *mTrapApplicationHLayout;
    QVBoxLayout *mTrapApplicationVLayout;
    QHBoxLayout *mSelectContentLayout;
    QLabel *mSelectContentLabel;
    QComboBox *mSelectContentCombobox;

    WBWebView *mTrapContentPreview;
    UBWebTrapMouseEventMask *mTrapingWidgetMask;

    QHBoxLayout *mApplicationNameLayout;
    QLabel *mApplicationNameLabel;
    QLineEdit *mApplicationNameEdit;
    QToolButton *mShowDisclaimerToolButton;
    QList<QToolButton *> mTrapButtons;
    QWebHitTestResult mLastHitTestResult;

};

#endif //WB_TRAP_WEB_PAGE_CONTENT_H
