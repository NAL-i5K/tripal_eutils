<?php

class EUtilsBioProjectParser implements EUtilsParserInterface{

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return array|mixed
   * @throws \Exception
   */
  public function parse(SimpleXMLElement $xml) {
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
          throw new Exception('Unknown tag ' . $key);
      }
    }

    return $info;
  }

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public  function bioProjectSubmission(SimpleXMLElement $xml) {
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
}
