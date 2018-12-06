<?php

/**
 * Class EUtils
 *
 * Factory class which returns the appropriate NCBI resource provider.
 *
 */
class EUtils{

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
