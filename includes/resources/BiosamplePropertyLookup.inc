<?php

/**
 * Fetch the published biosample attributes to feed our property list.
 *
 * @group resources
 */
class BiosamplePropertyLookup {

  /**
   * Looks up all attributes.
   *
   * @param string $url
   *   Optional URL string to lookup.
   *
   * @return array
   *   Array of terms keyed by harmonized (machine name)
   *   with human readable label and definition.
   */
  public function lookupAll(string $url = NULL) {
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

      // We could look at Package if we care where the term comes from.
      $terms[(string) $harmonized_name] = [
        'label' => (string) $term_label,
        'def' => (string) $definition,
      ];
    }

    return $terms;
  }

}
