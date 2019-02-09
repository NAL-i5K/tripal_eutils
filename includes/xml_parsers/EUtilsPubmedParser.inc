<?php

/**
 * Class EUtilsPubmedParser.
 *
 *
 * @ingroup parsers
 */
class EUtilsPubmedParser implements EUtilsParserInterface {

  /**
   * Parse an NCBI Pubmed XML.  Uses the core parser code.
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

    module_load_include('inc', 'tripal_chado', '/includes/loaders/tripal_chado.pub_importer_PMID');

    // Convert back to string for API.
    return tripal_pub_PMID_parse_pubxml($xml->PubmedArticle->asXML());

  }

}
