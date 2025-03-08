<?php

/**
 * Class EUtilsPubmedParser.
 *
 *
 * @ingroup parsers
 */
class EUtilsPubmedParser implements EUtilsParserInterface {

  /**
   * Parse an NCBI Pubmed XML. Uses the core parser code.
   *
   * @param \SimpleXMLElement $xml
   *   Simple XML Element.
   *
   * @return array|mixed
   *   Array.
   *
   * @throws \Exception
   */
  public function parse(SimpleXMLElement $xml) {
    $pub_library_manager = \Drupal::service('tripal.pub_library');
    $pubmed_plugin = $pub_library_manager->createInstance('tripal_pub_library_PMID', []);
    $data = $pubmed_plugin->parse($xml->PubmedArticle->asXML());
    return $data;
  }

}
