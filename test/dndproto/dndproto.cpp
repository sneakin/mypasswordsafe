#include <iostream>
#include <qapplication.h>
#include <qlistview.h>
#include <qdragobject.h>
#include <qpixmap.h>
#include <qlayout.h>
#include <qdom.h>
#include <qmessagebox.h>
#include "dndproto.hpp"

using namespace std;

unsigned int Data::id_counter = 0;

Data::Data()
  : id(id_counter++)
{
}

Data::Data(const QString &_name, const QString &_desc)
  : id(id_counter++), name(_name), desc(_desc)
{
}


static QString dnd_mimetype = "app/dndtest";

DragObject::DragObject(QWidget *src)
  : QDragObject(src), data("list")
{
}

const char *DragObject::format(int i) const
{
  if(i == 0)
    return dnd_mimetype;
  else
    return NULL;
}

bool DragObject::provides(const char *mime_type) const
{
  if(dnd_mimetype == QString(mime_type))
    return true;
  else
    return false;
}

QByteArray DragObject::encodedData(const char *mime_type) const
{
  if(dnd_mimetype == QString(mime_type)) {
    return data.toCString();
  }
  else {
    return QByteArray(0);
  }
}

bool DragObject::canDecode(const QMimeSource *src)
{
  return src->provides(dnd_mimetype);
}

bool DragObject::decode(const QMimeSource *src, QDomDocument &xml)
{
  if(canDecode(src)) {
    xml.setContent(src->encodedData(dnd_mimetype));
    return true;
  }
  return false;
}

DragItem::DragItem(QWidget *src, ListItem *item)
  : DragObject(src), item_data(item->data)
{
  addItem(item);
}

const char *DragItem::format(int i) const
{
  if(i == 0) {
    return "text/plain";
  }
  else {
    return DragObject::format(i - 1);
  }
}

bool DragItem::provides(const char *mime_type) const
{
  if(QString(mime_type) == QString("text/plain"))
    return true;
  else
    return DragObject::provides(mime_type);
}

QByteArray DragItem::encodedData(const char *mime_type) const
{
  if(QString(mime_type) == QString("text/plain")) {
    return item_data.name.utf8();
  }
  else {
    return DragObject::encodedData(mime_type);
  }
}

bool DragItem::canDecode(const QMimeSource *src)
{
  return (src->provides("text/plain") ||
	  DragObject::canDecode(src));
}

void DragItem::addItem(ListItem *item)
{
  data.appendChild(item->toXml(data));
}



DragGroup::DragGroup(QWidget *src, ListGroup *group)
  : DragObject(src)
{
  addGroup(group);
}

void DragGroup::addGroup(ListGroup *group)
{
  data.appendChild(group->toXml(data));
}



const int ListItem::RTTI = 2000;

ListItem::ListItem(QListView *parent, const QString &name, const QString &desc)
  : QListViewItem(parent)
{
  init(name, desc);
}

ListItem::ListItem(QListViewItem *parent, const QString &name, const QString &desc)
  : QListViewItem(parent)
{
  init(name, desc);
}

void ListItem::init(const QString &name, const QString &desc)
{
  data.name = name;
  data.desc = desc;
  
  setDragEnabled(true);
  setDropEnabled(true);
  
  //QPixmap pix(folder_icon);
  //setPixmap(0, pix);
}

int ListItem::rtti() const
{
  return RTTI;
}

bool ListItem::acceptDrop(const QMimeSource *mime) const
{
  return DragObject::canDecode(mime);
}

void ListItem::dropped(QDropEvent *event)
{
  ListView *view = (ListView *)listView();
  view->dropped(event, (ListGroup *)parent());
}

QString ListItem::text(int col) const
{
  switch(col) {
  case 0:
    return data.name;
  case 1:
    return data.desc;
  default:
    return QString::null;
  }
}

void ListItem::fromXml(QDomElement &elem)
{
  if(elem.tagName() == "item") {
    QDomNode n = elem.firstChild();
    for(; !n.isNull(); n = n.nextSibling()) {
      if(n.isElement()) {
	QDomElement e(n.toElement());
	QString tag_name(e.tagName());
	if(tag_name == "name") {
	  QDomNode name = e.firstChild();
	  data.name = name.toText().data();
	}
	else if(tag_name == "desc") {
	  QDomNode desc = e.firstChild();
	  data.desc = desc.toText().data();
	}
      }
    }
  }
}

QDomElement ListItem::toXml(QDomDocument &doc)
{
  QDomElement ret = doc.createElement("item");
  QDomElement tag = doc.createElement("name");
  QDomText text = doc.createTextNode(data.name);

  tag.appendChild(text);
  ret.appendChild(tag);

  tag = doc.createElement("desc");
  text = doc.createTextNode(data.desc);

  tag.appendChild(text);
  ret.appendChild(tag);

  return ret;
}

const int ListGroup::RTTI = 2001;

ListGroup::ListGroup(QListView *parent, const QString &name)
  : QListViewItem(parent, name)
{
  init();
}

ListGroup::ListGroup(QListViewItem *parent, const QString &name)
  : QListViewItem(parent, name)
{
  init();
}

void ListGroup::init()
{
  setDragEnabled(true);
  setDropEnabled(true);
}

int ListGroup::rtti() const
{
  return RTTI;
}

bool ListGroup::acceptDrop(const QMimeSource *mime) const
{
  return DragObject::canDecode(mime);
}

void ListGroup::dropped(QDropEvent *event)
{
  ListView *view = (ListView *)listView();
  view->dropped(event, this);
}

void ListGroup::fromXml(QDomElement &elem)
{
  if(elem.tagName() == "group") {
    QString name = elem.attribute("name");
    setText(0, name);

    QDomNode n = elem.firstChild();
    for(; !n.isNull(); n = n.nextSibling()) {
      if(n.isElement()) {
	QDomElement e = n.toElement();
	QString tag_name = e.tagName();
	if(tag_name == "item") {
	  ListItem *item = new ListItem(this, "", "");
	  item->fromXml(e);
	}
	else if(tag_name == "group") {
	  ListGroup *group = new ListGroup(this, e.attribute("name"));
	  group->fromXml(e);
	}
      }
    }
  }
}

QDomElement ListGroup::toXml(QDomDocument &doc)
{
  QDomElement group = doc.createElement("group");
  group.setAttribute("name", text(0));

  QListViewItem *item = firstChild();
  for(; item != NULL; item = item->nextSibling()) {
    if(item->rtti() == ListItem::RTTI) {
      ListItem *li = (ListItem *)item;
      group.appendChild(li->toXml(doc));
    }
    else if(item->rtti() == ListGroup::RTTI) {
      ListGroup *gr = (ListGroup *)item;
      group.appendChild(gr->toXml(doc));
    }
  }

  return group;
}


ListView::ListView(QWidget *parent, const char *name)
  : QListView(parent, name)
{
  setAllColumnsShowFocus(true);
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);
  setRootIsDecorated(true);
  
  addColumn("Name");
  addColumn("Description");

  connect(this, SIGNAL(dropped(QDropEvent *)), this, SLOT(dropped(QDropEvent *)));
}

ListItem *ListView::findItem(unsigned int id)
{
  QListViewItemIterator it(this);
  while(it.current()) {
    QListViewItem *item = *it;

    if(item->rtti() == ListItem::RTTI) {
      ListItem *list_item = (ListItem *)item;
      if(list_item->data.id == id)
	return list_item;
    }

    it++;
  }

  return NULL;
}

void ListView::startDrag()
{
  cout << name() << ": Drag started" << endl;
  m_target_is_child = false;
  m_target_parent = NULL;

  QListViewItem *item = selectedItem();
  DragObject *d; // = new DragObject(viewport());

  if(item->rtti() == ListItem::RTTI) {
    ListItem *i = (ListItem *)item;
    //d->setPixmap(*i->pixmap(0));
    d = new DragItem(viewport(), i);
  }
  else if(item->rtti() == ListGroup::RTTI) {
    ListGroup *g = (ListGroup *)item;
    //d->addGroup(g);
    d = new DragGroup(viewport(), g);
  }
  else {
    return;
  }

  bool drag_ret = d->drag();
  cout << "Drag ret = " << drag_ret << endl;

  if(drag_ret) {
    if(m_target_is_child) {
      QMessageBox::information(this, "Sorry", "Sorry, but items can't be dragged into their children");
    }
    else {
      delete item;
    }
  }
  else if(!drag_ret && m_target_is_child == true) {
    cout << "Copy item to child" << endl;
    addDraggedData(d, m_target_parent);
  }

  cout << name() << ": Drag ended" << endl;
}

bool ListView::isTargetChild(QDropEvent *event, ListGroup *group)
{
  // prevent incest
  cout << event->action() << endl;
  if(event->source() == viewport()) {
    cout << "Warning!!" << endl;
    QListViewItem *item = currentItem();

     // make sure this isn't a child of item
     QListViewItem *p = group;
     for(; p != NULL; p = p->parent()) {
       if(p == item) {
	 cout << "The dragged item is a parent" << endl;
	 m_target_is_child = true;
	 return true;
       }
     }
  }

  return false;
}

void ListView::dropped(QDropEvent *event, ListGroup *group)
{
  cout << "Dropped" << endl;
  if(DragObject::canDecode(event)) {
    cout << "DragObject dropped" << endl;

    m_target_parent = group;
    if(isTargetChild(event, group)) {
      return;
    }

    addDraggedData(event, group);
  }
  else {
    event->ignore();
  }
}

void ListView::addDraggedData(const QMimeSource *drag, QListViewItem *parent)
{
  QDomDocument xml("list");
  if(DragObject::decode(drag, xml)) {
    QDomNode n = xml.firstChild();
    for(; !n.isNull(); n = n.nextSibling()) {
      if(n.isElement()) {
	QDomElement elem = n.toElement();
	QString tag_name = elem.tagName();
	if(tag_name == "item") {
	  ListItem *item;
	  if(parent == NULL)
	    item = new ListItem(this, "", "");
	  else
	    item = new ListItem(parent, "", "");
	  item->fromXml(elem);
	}
	else if(tag_name == "group") {
	  ListGroup *new_group;
	  if(parent == NULL)
	    new_group = new ListGroup(this, "");
	  else
	    new_group = new ListGroup(parent, "");
	  new_group->fromXml(elem);
	}
      }
    }
  }
}

void ListView::dropped(QDropEvent *event)
{
  dropped(event, NULL);
}

void populate(QListView *list)
{
  static QString strings[][2] = {
    { "Hello", "World" },
    { "GNU", "Linux" },
    { "Good", "Bye" },
    { QString::null, QString::null }
  };

  QDomDocument doc("doc");
  QDomElement doc_elem = doc.createElement("list");
  doc.appendChild(doc_elem);

  for(int i = 0; i < 5; i++) {
    QListViewItem *group = new ListGroup(list,
					QString("%1").arg(i));
    for(int j = 0; !strings[j][0].isEmpty(); j++) {
      (void)new ListItem(group, strings[j][0], strings[j][1]);
    }
    QDomElement elem = ((ListGroup *)group)->toXml(doc);
    doc_elem.appendChild(elem);
  }
}

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  QWidget main(NULL, "main");
  QHBoxLayout layout(&main);

  ListView left_list(&main, "left");
  ListView right_list(&main, "right");

  layout.addWidget(&left_list);
  layout.addWidget(&right_list);

  left_list.show();
  right_list.show();

  main.resize(400, 320);
  main.show();

  populate(&left_list);
  populate(&right_list);

  app.connect(&app, SIGNAL(lastWindowClosed()), &app, SLOT(quit()));
  return app.exec();
}
