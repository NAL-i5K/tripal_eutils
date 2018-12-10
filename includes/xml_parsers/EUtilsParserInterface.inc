<?php

/**
 *
 */
interface EUtilsParserInterface {

  /**
   * @param \SimpleXMLElement $xml
   *
   * @return mixed
   */
  public function parse(SimpleXMLElement $xml);

}
