<?php

class EUtilsAssemblyParser implements EUtilsParserInterface {


  public function parse(SimpleXMLElement $xml) {

    $info = [];

    //skip to DocumentSummary

    $xml = $xml->DocumentSummarySet;

    $xml = $xml->DocumentSummary;

    $children = $xml->children();
    foreach ($children as $key => $child) {

      switch ($key) {

        case 'Meta':

          $info['attributes'] = $this->parseMeta($child);

          break;

        case 'AssemblyName':
          $info['name'] = (string) $child;
          break;

        case 'AssemblyDescription':
          $info['description'] = (string) $child;
          break;

        case 'AssemblyAccession':
          $info['accessions'][] = ['Assembly' => (string) $child];


        case 'default':
          $info['ignored'] = $child->getName();
          break;
      }

    }

    return $info;

  }


  /**
   * Parse the <Meta> tag, which containts CDATA but all of the Assembly props.
   * @param $x
   *
   * @return array
   */
  public function parseMeta($x) {

    $list = [];

    $trimmed = trim((string) $x);

    //Wrap this in a root in case its malformed.

    $wrapped = '<wrap>' . $trimmed . '</wrap>';

    $meta = simplexml_load_string($wrapped);

    $children = $meta->children();

    foreach ($children as $key => $child) {


      switch ($key) {

        case 'Stats':
          $list['stats'] = $this->processFinalChildren($child, 'category');
          break;

        case 'FtpSites':
          $list['files'] = $this->processFinalChildren($child, 'type');
        case 'default':
      }
    }

    return $list;
  }


  /**
   * Process terminal level children (IE a flat array of children).
   * @param $x - xml of children
   * @param $key - attribute key to derive the label from.
   *
   * @return array
   */
  private function processFinalChildren($x, $key) {


    $results = [];

    $children = $x->children();

    foreach ($children as $child){

      $attributes = $child->attributes();
      $label = $attributes[$key];

      $stats[$label] = (string) $child;
    }

    return $results;

  }


}

