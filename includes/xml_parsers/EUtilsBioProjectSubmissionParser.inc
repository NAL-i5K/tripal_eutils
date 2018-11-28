<?php

class EUtilsBioProjectSubmissionParser implements EUtilsParserInterface{

  public function parse(SimpleXMLElement $xml) {
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
