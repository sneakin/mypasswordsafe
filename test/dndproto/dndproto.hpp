#include <qdragobject.h>
#include <qlistview.h>
#include <qvaluelist.h>
#include <qdom.h>
#include <qptrlist.h>

class QDomElement;

class Data
{
public:
  static unsigned int id_counter;

  Data();
  Data(const QString &, const QString &);

  unsigned int id;
  QString name, desc;
};

class ListItem;
class ListGroup;

class DragObject: public QDragObject
{
  Q_OBJECT;

public:
  DragObject(QWidget *src = 0);

  void add(QListViewItem *item);

  virtual const char *format(int i = 0) const;
  virtual bool provides(const char *mime_type) const;
  virtual QByteArray encodedData(const char *mime_type) const;
  static bool canDecode(const QMimeSource *src);
  static bool decode(const QMimeSource *src, QDomDocument &xml);

  QPtrList<QListViewItem> items; // you probably want to store the actual data or pointers to it
  QString plain_text;
};

class ListItem: public QListViewItem
{
public:
  static const int RTTI;

  ListItem(QListView *parent, const QString &name, const QString &desc);
  ListItem(QListViewItem *parent, const QString &name, const QString &desc);

  void init(const QString &name, const QString &desc);
  int rtti() const;

  virtual bool acceptDrop(const QMimeSource *mime) const;
  virtual void dropped(QDropEvent *event);

  QString text(int col = 0) const;

  void fromXml(QDomElement &);
  QDomElement toXml(QDomDocument &doc) const;

  Data data;
};

class ListGroup: public QListViewItem
{
public:
  static const int RTTI;

  ListGroup(QListView *parent, const QString &name);
  ListGroup(QListViewItem *parent, const QString &name);

  void init();
  int rtti() const;

  virtual bool acceptDrop(const QMimeSource *mime) const;
  virtual void dropped(QDropEvent *event);

  void fromXml(QDomElement &);
  QDomElement toXml(QDomDocument &doc) const;
};

class ListView: public QListView
{
  Q_OBJECT;

public:
  ListView(QWidget *parent = NULL, const char *name = NULL);

  ListItem *findItem(unsigned int id);

  void startDrag();
  void dropped(QDropEvent *, ListGroup *);

protected:
  bool isTargetChild(QDropEvent *event, ListGroup *target);
  void addDraggedData(const QMimeSource *drag, QListViewItem *parent);

private:
  DragObject *createDragObject();
  void deleteSelectedItems();

  bool m_target_is_child;
  QListViewItem *m_target_parent;

private slots:
  void dropped(QDropEvent *);
};
