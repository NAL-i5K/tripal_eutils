<?php

/**
 * Interface EUtilsFactoryInterface.
 */
interface EUtilsFactoryInterface {

  /**
   * Get the appropriate class.
   *
   * @param string $db
   *   The name of the DB.
   *
   * @return mixed
   */
  public function get(string $db);

}
