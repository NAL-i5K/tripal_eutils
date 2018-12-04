<?php

class BiosamplePropertyLookup{

  /**
   * @param string $url
   *
   * @return array
   */
  public function lookupAll($url = NULL) {
    $request = new EUtilsRequest();

    if (is_null($url)) {
      $url = 'https://www.ncbi.nlm.nih.gov/biosample/docs/attributes';
    }

    $response = $request->setBaseURL($url)->addParam('format', 'xml')->get();

    $terms = [];
    $xml = $response->xml();

    $children = $xml->children();

    foreach ($children as $child) {

      $term_label = $child->Name;
      $harmonized_name = $child->HarmonizedName;
      $definition = $child->Description;

      //we could look at Package if we care where the term comes from.

      $terms[(string) $harmonized_name] = [
        'label' => (string) $term_label,
        'def' => (string) $definition,
      ];
    }

    return $terms;
  }
}
