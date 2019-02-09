<?php

/**
 * EUtilsFormatterFactory.
 *
 * Creates a formatter given the db.
 *
 * @ingroup formatters
 */
class EUtilsFormatterFactory implements EUtilsFactoryInterface {

  /**
   * A map of db to formatter class.
   *
   * @var array
   */
  protected $formatter = [
    'biosample' => EUtilsBioSampleFormatter::class,
    'assembly' => EutilsAssemblyFormatter::class,
    'bioproject' => EutilsBioProjectFormatter::class,
    'pubmed' => EUtilsPubmedFormatter::class,

  ];

  /**
   * @param string $db
   *   The database name.
   *
   * @return \EUtilsFormatter
   *   The formatter for the given DB.
   *
   * @throws \Exception
   */
  public function get(string $db) {
    if (!isset($this->formatter[$db])) {
      throw new Exception('Unable to find formatter for the given db: ' . $db);
    }

    return new $this->formatter[$db]();
  }

}
