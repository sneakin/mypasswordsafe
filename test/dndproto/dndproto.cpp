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


static QString dnd_mimetype = "app/dndtest"; //<! this is the drag object's mimetype (for this app)

DragObject::DragObject(QWidget *src)
  : QDragObject(src)
{
}

/** Adds data to the drag object.
 */
void DragObject::add(QListViewItem *item)
{
  items.append(item);

  if(item->rtti() == ListItem::RTTI) {
    plain_text = ((ListItem *)item)->data.name;
  }
}

/** Used to get the supported mime types.
 */
const char *DragObject::format(int i) const
{
  if(i == 0)
    return dnd_mimetype;
  else if(i == 1)
    return "text/plain";
  else
    return NULL;
}

/** Checks to see if a mime type is supported
 */
bool DragObject::provides(const char *mime_type) const
{
  QString mime(mime_type);

  if(dnd_mimetype == mime)
    return true;
  else if(QString("text/plain") == mime && !plain_text.isEmpty())
    return true;
  else
    return false;
}

/** Returns a byte array in the requested mime type of the data the drag
 * object is storing.
 */
QByteArray DragObject::encodedData(const char *mime_type) const
{
  QString mime(mime_type);

  if(dnd_mimetype == mime) {
    QDomDocument data("list");
    QDomElement root = data.createElement("drag");
    data.appendChild(root);

    QPtrListIterator<QListViewItem> it(items);
    for(; it.current(); ++it) {
      QListViewItem *item = it.current();

      if(item->rtti() == ListItem::RTTI) {
	root.appendChild(((ListItem *)item)->toXml(data));
      }
      else if(item->rtti() == ListGroup::RTTI) {
	root.appendChild(((ListGroup *)item)->toXml(data));
      }
    }

    cout << "encodedData:" << endl << data.toString() << endl;
    return data.toCString();
  }
  else if(QString("text/plain") == mime && !plain_text.isEmpty()) {
    return plain_text.utf8();
  }
  else {
    return QByteArray(0);
  }
}

/** A helper method.
 */
bool DragObject::canDecode(const QMimeSource *src)
{
  return src->provides(dnd_mimetype);
}

/** Decodes the data from the mime source into the QDomDocument.
 */
bool DragObject::decode(const QMimeSource *src, QDomDocument &xml)
{
  if(canDecode(src)) {
    xml.setContent(src->encodedData(dnd_mimetype));
    return true;
  }
  return false;
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

/** This is called to determine if the data stored in a drag
 * can be accepted by the list item.
 */ 
bool ListItem::acceptDrop(const QMimeSource *mime) const
{
  return DragObject::canDecode(mime);
}

/** This is called when a drop event occurs. I pass it up to the view
 * to handle it, so I don't duplcate a bunch of code.
 */
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

/** This builds an item from XML.
 */
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

/** This turns an item into XML.
 */
QDomElement ListItem::toXml(QDomDocument &doc) const
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

/** This is called to determine if the data stored in a drag
 * can be accepted by the list item.
 */ 
bool ListGroup::acceptDrop(const QMimeSource *mime) const
{
  return DragObject::canDecode(mime);
}

/** This is called when a drop event occurs. I pass it up to the view
 * to handle it, so I don't duplcate a bunch of code.
 */
void ListGroup::dropped(QDropEvent *event)
{
  ListView *view = (ListView *)listView();
  view->dropped(event, this);
}

/** This builds an XML element from the group and all of its
 * children.
 */
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

/** This decodes XML, sets the group's data, and creates any children items.
 */
QDomElement ListGroup::toXml(QDomDocument &doc) const
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
  setRootIsDecorated(true);

  // These next two lines turn on drop support
  setAcceptDrops(true);
  viewport()->setAcceptDrops(true);

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

/** This creates the drag object.
 * It iterates over the selected items and adds them to the drag object.
 */
DragObject *ListView::createDragObject()
{
  DragObject *d = new DragObject(viewport());
  QListViewItemIterator it(this, QListViewItemIterator::Selected
		            | QListViewItemIterator::DragEnabled
			    | QListViewItemIterator::Visible);
  while(it.current()) {
    d->add(it.current());
    it++;
  }

  return d;
}

/** This was used when I was trying out multiple item
 * drags.
 */
void ListView::deleteSelectedItems()
{
  QListViewItemIterator it(this, QListViewItemIterator::Selected
		            | QListViewItemIterator::DragEnabled
			    | QListViewItemIterator::Visible);
  while(it.current()) {
    delete it.current();
    it++;
  }
}

/** This is called when a drag starts. It creates the
 * drag object, and tells it to start the drag.
 */
void ListView::startDrag()
{
  cout << name() << ": Drag started" << endl;

  // these get set in isTargetChild
  m_target_is_child = false;
  m_target_parent = NULL;

  // create the drag object
  QListViewItem *item = currentItem();
  DragObject *d = createDragObject();

  // start the drag
  bool drag_ret = d->drag();
  cout << "Drag ret = " << drag_ret << endl;

  // drag was a move
  if(drag_ret) {
    // prevent drops from doing anything when they occur on a child
    // of the dragged object
    if(m_target_is_child) {
      QMessageBox::information(this, "Sorry", "Sorry, but items can't be dragged into their children");
    }
    else {
      // remove the item from the list
      delete item;
      //deleteSelectedItems();
    }
  }
  // drag was a copy onto a child
  else if(!drag_ret && m_target_is_child == true) {
    cout << "Copy item to child" << endl;
    addDraggedData(d, m_target_parent);
  }

  cout << name() << ": Drag ended" << endl;
}

/** Checks if the drop occured on a child of the item
 * that got dragged. If the target is a child,
 * m_target_is_child gets set to true.
 */
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

/** This is called from the items when data gets dropped
 * onto them. It sets m_target_parent to group for future
 * use when the drag gets complete to determine if data
 * needs to be deleted.
 */
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

/** Decodes the drop event into XML, and adds the items
 * that are encoded in the XML to the list.
 */
void ListView::addDraggedData(const QMimeSource *drag, QListViewItem *parent)
{
  QDomDocument xml("list");
  if(DragObject::decode(drag, xml)) {
    cout << "Dragged data:" << endl << xml.toString() << endl;
    QDomElement root = xml.documentElement();

    QDomNode n = root.firstChild();
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

/** A drop occured where there was no item.
 */
void ListView::dropped(QDropEvent *event)
{
  dropped(event, NULL);
}



/** Add some items to a list.
 */
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
