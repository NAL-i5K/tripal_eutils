<?php

/**
 * Class EUtilsXMLParserFactory
 *
 * This is the base EUTILS XML parser class.
 * The plan is to extend this base class to be specific for each DB type.
 */
class EUtilsXMLParserFactory {

  /**
   *
   * @param $db
   * @param $xml
   *
   * @return array|null
   * @throws \Exception
   */
  public function get($db, $xml) {

    $db = strtolower($db);
    switch ($db) {
      case 'bioproject':
        $parser = $this->bioproject($xml);
        break;

      case 'biosample':
        $parser = $this->biosample($xml);
        break;

      case 'assembly':
        $parser = $this->assembly($xml);
        break;
    }
    return $parser;

  }

  /**
   * Parse BioProject XML.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   *
   * @throws \Exception
   */
  public function bioProject(SimpleXMLElement $xml) {
    return (new EUtilsBioProjectParser())->parse($xml);
  }

  /**
   * Parse BioSample XML.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function bioSample(SimpleXMLElement $xml) {
    return (new EUtilsBioSampleParser())->parse($xml);
  }

  /**
   * Parse Assembly XML.
   *
   * @param \SimpleXMLElement $xml
   *
   * @return array
   */
  public function assembly(SimpleXMLElement $xml) {
    return (new EUtilsAssemblyParser())->parse($xml);
  }

}
