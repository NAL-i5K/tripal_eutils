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

    $doc = $x->RecordSet;

    if (!property_exists($x, 'DocumentSummary')) {
      print("No document summary key!");
      return FALSE;
    }
    $doc = $x->DocumentSummary;

    foreach ($doc->children() as $key => $child) {


      // Go through the main keys.

      switch ($key) {
        case "Submission":

          $this->bioproject_submission($child);


          break;
        case "Project":
          $this->bioproject_project($child);
          break;

        default:
          var_dump(["default", $key]);
          break;
      }
    }
  }


  /**
   * Accepts XML with the root key Submission
   *
   * @param $xml
   */
  private function bioproject_submission($xml) {

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

  private function bioproject_project($xml) {

  }

}