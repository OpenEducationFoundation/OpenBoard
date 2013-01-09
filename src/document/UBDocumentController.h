/*
 * Copyright (C) 2012 Webdoc SA
 *
 * This file is part of Open-Sankoré.
 *
 * Open-Sankoré is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation, version 2,
 * with a specific linking exception for the OpenSSL project's
 * "OpenSSL" library (or with modified versions of it that use the
 * same license as the "OpenSSL" library).
 *
 * Open-Sankoré is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with Open-Sankoré; if not, see
 * <http://www.gnu.org/licenses/>.
 */


#ifndef UBDOCUMENTCONTROLLER_H_
#define UBDOCUMENTCONTROLLER_H_

#include <QtGui>
#include "document/UBDocumentContainer.h"

namespace Ui
{
    class documents;
}

#include "gui/UBMessageWindow.h"

class UBGraphicsScene;
class QDialog;
class UBDocumentProxy;
class UBBoardController;
class UBThumbnailsScene;
class UBDocumentGroupTreeItem;
class UBDocumentProxyTreeItem;
class UBMainWindow;
class UBDocumentToolsPalette;


class UBDocumentTreeNode
{
public:
    friend class UBDocumentTreeModel;

    enum Type {
        Catalog
        , Document
    };

    UBDocumentTreeNode(Type pType, const QString &pName, const QString &pDisplayName = QString(), UBDocumentProxy *pProxy = 0);
    UBDocumentTreeNode() : mType(Catalog), mParent(0), mProxy(0) {;}
    ~UBDocumentTreeNode();

    QList<UBDocumentTreeNode*> children() const {return mChildren;}
    UBDocumentTreeNode *parentNode() {return mParent;}
    Type nodeType() const {return mType;}
    QString nodeName() const {return mName;}
    QString displayName() const {return mDisplayName;}
    void setNodeName(const QString &str) {mName = str; mDisplayName = str;}
    void addChild(UBDocumentTreeNode *pChild);
    void removeChild(int index);
    UBDocumentTreeNode *moveTo(const QString &pPath);
    UBDocumentProxy *proxyData() const {return mProxy;}
    bool isRoot() {return !mParent;}
    bool isTopLevel() {return mParent && !mParent->mParent;}
    UBDocumentTreeNode *clone();
    QString dirPathInHierarchy();

private:
    Type mType;
    QString mName;
    QString mDisplayName;
    UBDocumentTreeNode *mParent;
    QList<UBDocumentTreeNode*> mChildren;
    QPointer<UBDocumentProxy> mProxy;
};
Q_DECLARE_METATYPE(UBDocumentTreeNode*)

class UBDocumentTreeModel : public QAbstractItemModel {
    Q_OBJECT

public:
    enum eAncestors {
        aMyDocuments
        , aUntitledDocuments
        , aModel
        , aTrash
    };

    enum eCopyMode {
        aReference
        , aContentCopy
    };

    UBDocumentTreeModel(QObject *parent = 0);
    ~UBDocumentTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);
    Qt::ItemFlags flags ( const QModelIndex & index ) const;
    Qt::DropActions supportedDropActions() const {return Qt::MoveAction | Qt::CopyAction;}
    QStringList mimeTypes() const;
    QMimeData *mimeData (const QModelIndexList &indexes) const;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent);
    bool removeRows(int row, int count, const QModelIndex &parent);

    QModelIndex indexForNode(UBDocumentTreeNode *pNode) const;
    QPersistentModelIndex persistentIndexForNode(UBDocumentTreeNode *pNode);
//    bool insertRow(int row, const QModelIndex &parent);
    void addNode(UBDocumentTreeNode *pFreeNode, UBDocumentTreeNode *pParent);
    QModelIndex addNode(UBDocumentTreeNode *pFreeNode, const QModelIndex &pParent);
    QPersistentModelIndex copyIndexToNewParent(const QModelIndex &source, const QModelIndex &newParent, eCopyMode pMode = aReference);
    void setCurrentNode(UBDocumentTreeNode *pNode) {mCurrentNode = pNode;}
    QModelIndex indexForProxy(UBDocumentProxy *pSearch) const;
    void setCurrentDocument(UBDocumentProxy *pDocument);
    void setRootNode(UBDocumentTreeNode *pRoot);
    UBDocumentProxy *proxyForIndex(const QModelIndex &pIndex) const;
    QString virtualDirForIndex(const QModelIndex &pIndex) const;
    QStringList nodeNameList(const QModelIndex &pIndex) const;
    bool newFolderAllowed(const QModelIndex &pIndex)  const;
    QModelIndex goTo(const QString &dir);
    bool inTrash(const QModelIndex &index) const;
    bool inModel(const QModelIndex &index) const;
    bool inUntitledDocuments(const QModelIndex &index) const;
    bool isCatalog(const QModelIndex &index) const {return nodeFromIndex(index)->nodeType() == UBDocumentTreeNode::Catalog;}
    bool isDocument(const QModelIndex &index) const {return nodeFromIndex(index)->nodeType() == UBDocumentTreeNode::Document;}
    bool isToplevel(const QModelIndex &index) const {return nodeFromIndex(index) ? nodeFromIndex(index)->isTopLevel() : false;}
    bool isConstant(const QModelIndex &index) const {return isToplevel(index) || (index == mUntitledDocuments);}
    UBDocumentProxy *proxyData(const QModelIndex &index) const {return nodeFromIndex(index)->proxyData();}
    void addDocument(UBDocumentProxy *pProxyData, const QModelIndex &pParent = QModelIndex());
    void addCatalog(const QString &pName, const QModelIndex &pParent);
    QList<UBDocumentProxy*> newDocuments() {return mNewDocuments;}
    void setNewName(const QModelIndex &index, const QString &newName);

    QPersistentModelIndex myDocumentsIndex() const {return mMyDocuments;}
    QPersistentModelIndex modelsIndex() const {return mModels;}
    QPersistentModelIndex trashIndex() const {return mTrash;}
    QPersistentModelIndex untitledDocumentsIndex() const {return mUntitledDocuments;}
    UBDocumentTreeNode *nodeFromIndex(const QModelIndex &pIndex) const;

signals:
    void indexChanged(const QModelIndex &newIndex, const QModelIndex &oldIndex);

private:
    UBDocumentTreeNode *mRootNode;
    UBDocumentTreeNode *mCurrentNode;

    UBDocumentTreeNode *findProxy(UBDocumentProxy *pSearch, UBDocumentTreeNode *pParent) const;
    QModelIndex pIndexForNode(const QModelIndex &parent, UBDocumentTreeNode *pNode) const;
    bool isDescendantOf(const QModelIndex &pPossibleDescendant, const QModelIndex &pPossibleAncestor) const;
    QPersistentModelIndex mRoot;
    QPersistentModelIndex mMyDocuments;
    QPersistentModelIndex mModels;
    QPersistentModelIndex mTrash;
    QPersistentModelIndex mUntitledDocuments;
    QList<UBDocumentProxy*> mNewDocuments;

};

class UBDocumentTreeMimeData : public QMimeData
{
    Q_OBJECT

    public:
        QList<QModelIndex> indexes() const {return mIndexes;}
        void setIndexes (const QList<QModelIndex> &pIndexes) {mIndexes = pIndexes;}

    private:
        QList<QModelIndex> mIndexes;
};

class UBDocumentTreeView : public QTreeView
{
    Q_OBJECT

public:
    UBDocumentTreeView (QWidget *parent = 0);

public slots:
    void setSelectedAndExpanded(const QModelIndex &pIndex, bool pExpand = true);
    void onModelIndexChanged(const QModelIndex &pNewIndex, const QModelIndex &pOldIndex);

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);


    UBDocumentTreeModel *fullModel() {return qobject_cast<UBDocumentTreeModel*>(model());}
    void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);

private:
    bool isAcceptable(const QModelIndex &dragIndex, const QModelIndex &atIndex);
    Qt::DropAction acceptableAction(const QModelIndex &dragIndex, const QModelIndex &atIndex);
};

class UBDocumentTreeItemDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:
    UBDocumentTreeItemDelegate(QObject *parent = 0);

private slots:
    void commitAndCloseEditor(QLineEdit *editor);

protected:
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    void setEditorData(QWidget *editor, const QModelIndex &index) const;
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;
    void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex &index) const;
};

class UBDocumentController : public UBDocumentContainer
{
    Q_OBJECT

    public:
        UBDocumentController(UBMainWindow* mainWindow);
        virtual ~UBDocumentController();

        void closing();
        QWidget* controlView();
        UBDocumentProxyTreeItem* findDocument(UBDocumentProxy* proxy);
        bool addFileToDocument(UBDocumentProxy* document);
        void deletePages(QList<QGraphicsItem*> itemsToDelete);
        int getSelectedItemIndex();

        bool pageCanBeMovedUp(int page);
        bool pageCanBeMovedDown(int page);
        bool pageCanBeDuplicated(int page);
        bool pageCanBeDeleted(int page);
        QString documentTrashGroupName(){ return mDocumentTrashGroupName;}
        QString defaultDocumentGroupName(){ return mDefaultDocumentGroupName;}

        void setDocument(UBDocumentProxy *document, bool forceReload = false);
        QModelIndex firstSelectedTreeIndex();

    signals:
        void exportDone();

    public slots:
        void createNewDocument();
        void createNewDocumentGroup();
        void deleteSelectedItem();
        void deleteIndexAndAssociatedData(const QModelIndex &pIndex);
        void renameSelectedItem();
        void openSelectedItem();
        void duplicateSelectedItem();
        void importFile();
        void moveSceneToIndex(UBDocumentProxy* proxy, int source, int target);
        void selectDocument(UBDocumentProxy* proxy, bool setAsCurrentDocument = true);
        void show();
        void hide();
        void showMessage(const QString& message, bool showSpinningWheel = false);
        void hideMessage();
        void toggleDocumentToolsPalette();
        void cut();
        void copy();
        void paste();
        void focusChanged(QWidget *old, QWidget *current);
        void updateActions();

    protected:
        virtual void setupViews();
        virtual void setupToolbar();
        void setupPalettes();
        bool isOKToOpenDocument(UBDocumentProxy* proxy);
        UBDocumentProxy* selectedDocumentProxy();
        UBDocumentProxy *firstSelectedTreeProxy();
        QList<UBDocumentProxy*> selectedProxies();
        QModelIndexList selectedTreeIndexes();
        UBDocumentProxyTreeItem* selectedDocumentProxyTreeItem();
        UBDocumentGroupTreeItem* selectedDocumentGroupTreeItem();
        QStringList allGroupNames();

        enum LastSelectedElementType
        {
            None = 0, Folder, Document, Page
        };

        LastSelectedElementType mSelectionType;

    private:
        QWidget *mParentWidget;
        UBBoardController *mBoardController;
        Ui::documents* mDocumentUI;
        UBMainWindow* mMainWindow;
        QWidget *mDocumentWidget;
        QPointer<UBMessageWindow> mMessageWindow;
        QAction* mAddFolderOfImagesAction;
        QAction* mAddFileToDocumentAction;
        QAction* mAddImagesAction;
        bool mIsClosing;
        UBDocumentToolsPalette *mToolsPalette;
        bool mToolsPalettePositionned;
        UBDocumentGroupTreeItem* mTrashTi;

        void moveDocumentToTrash(UBDocumentGroupTreeItem* groupTi, UBDocumentProxyTreeItem *proxyTi);
        void moveFolderToTrash(UBDocumentGroupTreeItem* groupTi);
        QString mDocumentTrashGroupName;
        QString mDefaultDocumentGroupName;

        UBDocumentProxy *mCurrentTreeDocument;

    private slots:
        void documentZoomSliderValueChanged (int value);
        void loadDocumentProxies();
        void TreeViewSelectionChanged(const QModelIndex &current, const QModelIndex &previous);
        void TreeViewSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
        void itemSelectionChanged();
        void exportDocument();
        void exportDocumentSet();
        void itemChanged(QTreeWidgetItem * item, int column);
        void thumbnailViewResized();
        void pageSelectionChanged();
        void selectionChanged();
        void documentSceneChanged(UBDocumentProxy* proxy, int pSceneIndex);
        void pageDoubleClicked(QGraphicsItem* item, int index);
        void thumbnailPageDoubleClicked(QGraphicsItem* item, int index);
        void pageClicked(QGraphicsItem* item, int index);
        void itemClicked(QTreeWidgetItem * item, int column );
        void addToDocument();
        void addDocumentInTree(UBDocumentProxy* pDocument);
        void updateDocumentInTree(UBDocumentProxy* pDocument);
        void addFolderOfImages();
        void addFileToDocument();
        void addImages();

        void refreshDocumentThumbnailsView(UBDocumentContainer* source);
};



#endif /* UBDOCUMENTCONTROLLER_H_ */
