<?php

/**
 * Class EUtilsXMLParser
 * This is the base EUTILS XML parser class.
 * The plan is to extend this base class to be specific for each DB type.
 */

class EUtilsXMLParser{

  //private $db = NULL;
  //
  ////XML, the eUTILS dom interpreted by simplexml
  //private $xml = NULL;
  //
  //public function setDB($db) {
  //  $this->db = $db;
  //}
  //public function loadXML($dom) {
  //
  //  $x = simplexml_import_dom($dom);
  //  $this->xml = $x;
  //}
  //
  ///**
  // * Returns the attributes used in this xml file.
  // *
  // * @return array
  // */
  //public function list_attributes() {
  //
  //  $list = [];
  //
  //  //The part of teh XML document we're going to search for.  should have an array of children with the attributes.
  //  $needle = NULL;
  //
  //  //The XML tag's attribute we will derive the name from.
  //  $attr_name = NULL;
  //
  //  switch ($this->db) {
  //
  //    case 'assembly':
  //      $needle = 'Stats';//or Meta?  Not sure....
  //      $attr_name = "category";
  //      break;
  //
  //    case 'biosample':
  //      $needle = 'Attributes';
  //
  //    case 'bioproject'://no attribute list
  //      break;
  //  }
  //  $xml = $this->xml;
  //
  //  if ($needle && $xml) {
  //
  //    //search anywhere in the xml object for the needle.
  //    $results = $xml->xpath('//' . $needle);
  //
  //    if ($results) {
  //
  //
  //      $children = $results->children();
  //
  //      foreach ($children as $child) {
  //
  //        $attributes = $child->attributes();
  //
  //        $label = $attributes[$attr_name];
  //        $value = (string) $child;
  //
  //        $list[] = ['label' => $label, 'value' => $value];
  //      }
  //    }
  //  }
  //
  //  return $list;
  //}
  //

  /**
   * Parse BioProject XML.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   * @throws \Exception
   */
  public function bioProject(SimpleXMLElement $xml) {
    return (new EUtilsBioProjectParser())->parse($xml);
  }

  /**
   * Parse BioSample XML.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function bioSample(SimpleXMLElement $xml) {
    return (new EUtilsBioSampleParser())->parse($xml);
  }
}
