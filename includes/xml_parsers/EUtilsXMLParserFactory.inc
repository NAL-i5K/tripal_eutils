<?php

/**
 * Class EUtilsXMLParserFactory.
 *
 * This is the base EUTILS XML parser class.
 * The plan is to extend this base class to be specific for each DB type.
 */
class EUtilsXMLParserFactory implements EUtilsFactoryInterface {

  /**
   * A map of DB to xml parsers.
   *
   * @var array
   */
  protected $parsers = [
    'bioproject' => EUtilsBioProjectParser::class,
    'biosample' => EUtilsBioSampleParser::class,
    'assembly' => EUtilsAssemblyParser::class,
    'pubmed' => EUtilsPubmedParser::class
  ];

  /**
   * Get the appropriate XML parser.
   *
   * @param string $db
   *   The name of the DB.
   *
   * @return \EUtilsParserInterface
   *
   * @throws \Exception
   */
  public function get(string $db) {
    $ldb = strtolower($db);

    if (!isset($this->parsers[$ldb])) {
      throw new Exception('Unable to find formatter for the given db: ' . $db);
    }

    return new $this->parsers[$ldb]();
  }

}
