#ifndef XMLABLE_HPP
#define XMLABLE_HPP

class QDomNode;
class QDomDocument;

/** An ABC for objects that can be converted to and from XML.
 * Just inherit from this class, and provide overrides for
 * fromXml and toXml. fromXml and toXml should be reversible.
 * For example:
 * \code
 * QDomNode xml = xmlable1.toXml(xml_doc);
 * xmlable2.fromXml(xmlable2);
 * assert(xmlable1 == xmlable2);
 * \endcode
 */
class Xmlable
{
public:
  /** Loads values for the object from XML.
   * @param node The QDomNode that's holding the XML data that
   *             will be used to setup the object.
   * @return true if the load succeded, false if not. False usually
   *         indicates a problem with the XML node.
   */
  virtual bool fromXml(const QDomNode &node) = 0;
  /** Converts the object into an XML node.
   * @param doc A QDomDocument that the QDomNode was created with.
   * @return A QDomNode representing the object.
   */
  virtual QDomNode toXml(QDomDocument &doc) const = 0;;
};

#endif /* XMLABLE_HPP */
