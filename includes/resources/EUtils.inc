<?php

/**
 * Class EUtils
 *
 * Factory class which returns the appropriate NCBI resource provider.
 *
 */
class EUtils {

  public function get($db, $accession) {

    $provider = $this->getResourceProvider($db);

    $xml = $provider->xml();

    $parser = new EUtilsXMLParser($db);
    $data = $parser->parse($xml);
    $repository = new EutilsRepository($db);

    $record = $repository->create($data);

    return $record;

  }

  /**
   * @param $db
   *
   * @return \EFetch|\ESearch|\ESummary
   * @throws \Exception
   */
  protected function getResourceProvider($db) {
    if ($db === 'assembly') {
      return new ESummary($db);
    }

    if ($db === 'pmc') {
      return new ESearch($db);
    }

    return new EFetch($db);
  }
}
