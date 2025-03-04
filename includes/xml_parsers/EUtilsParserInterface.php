<?php

/**
 * Eutils Parsers should accept XML and return an array.
 *
 * @defgroup parsers
 */
interface EUtilsParserInterface {

  /**
   * Parse XML into a keyed array.
   *
   * @param \SimpleXMLElement $xml
   *   XML returned from NCBI.
   *
   * @return array
   *   An array indexed by general content type.
   */
  public function parse(SimpleXMLElement $xml);

}
