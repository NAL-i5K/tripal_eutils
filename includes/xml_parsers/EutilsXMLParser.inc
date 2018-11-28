<?php

/**
 * Class EutilsXMLParser
 * This is the base EUTILS XML parser class.
 * The plan is to extend this base class to be specific for each DB type.
 */

class EutilsXMLParser {

  private $db = NULL;

  //XML, the eUTILS dom interpreted by simplexml
  private $xml = NULL;

  /**
   * EutilsXMLParser constructor.
   *
   * I think in the future a subclass will handle the db type.
   *
   * @param $db
   */
  public function __construct($db) {

    $this->db = $db;
  }

  public function loadXML($dom) {

    $x = simplexml_import_dom($dom);
    $this->xml = $x;
  }

  /**
   * Returns the attributes used in this xml file.
   * @return array
   */
  public function list_attributes() {

    $list = [];

    //The part of teh XML document we're going to search for.  should have an array of children with the attributes.
    $needle = NULL;

    //The XML tag's attribute we will derive the name from.
    $attr_name = NULL;

    switch ($this->db) {

      case 'assembly':
        $needle = 'Stats';//or Meta?  Not sure....
        $attr_name = "category";
        break;

      case 'biosample':
        $needle = 'Attributes';

      case 'bioproject'://no attribute list
        break;

    }
    $xml = $this->xml;

    if ($needle && $xml) {

      //search anywhere in the xml object for the needle.
      $results = $xml->xpath('//' . $needle);

      if ($results) {


        $children = $results->children();

        foreach ($children as $child) {

          $attributes = $child->attributes();

          $label = $attributes[$attr_name];
          $value = (string) $child;

          $list[] = ['label' => $label, 'value' => $value];

        }

      }
    }

    return $list;
  }


  /**
   * Accepts XML with the root key Submission
   *
   * @param $xml
   */
  private function bioprojectSubmission($xml) {

    $info = [];

    //First get attributes of parent.

    $attributes = $xml->attributes();

    //for example:
    // ["last_update"]=>
    //    string(10) "2018-11-21"
    //    ["submission_id"]=>
    //    string(10) "SUB4827559"
    //    ["submitted"]=>
    //    string(10) "2018-11-21"

    //Now deal with children

    $children = $xml->children();

    foreach ($children as $key => $child) {

      //Keys so far are description and action.  No one cares about action?
      if ($key == 'Description') {
        $org = $child->Organization->Name;
        $info['organization'] = (string) $org;
      }
    }
    return $info;
  }

  /**
   * @param $xml
   *
   * @return array
   * @throws \Exception
   */
  private function bioprojectProject($xml) {
    $info = [];

    //dont expect parent attributes to matter
    // $attributes = $xml->attributes();

    $children = $xml->children();
    foreach ($children as $key => $child) {
      switch ($key) {

        case 'ProjectDescr':
          //Information about the project itself.  Includes title, description

          break;

        case 'ProjectType':
          //Includes organism, metadata for project.

          $target = $child->ProjectTypeSubmission->Target;

          if (!$target) {
            break;
          }

          $organism = $target->Organism;

          if (!$organism) {
            break;
          }

          $attributes = $organism->attributes();

          //What about other children and their attributes?

          $info['type']['organism']['taxID'] = $attributes['taxID'];

          break;

        case 'ProjectID':
          //Accession info for the project.  Should match what was submitted, thats about it.

          break;

        default:
          throw new Exception('Unknown tag '.$key);
      }
    }

    return $info;
  }
}
