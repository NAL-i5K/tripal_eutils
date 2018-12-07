<?php

class EutilsRepositoryFactory {

  /**
   * @param $db
   * @return array|null
   * @throws \Exception
   */
  public function get($db) {

    $db = strtolower($db);
    switch ($db) {
      case 'bioproject':
        return new EUtilsBioProjectRepository();
        break;
      case 'biosample':
        return new EUtilsBioSampleRepository();

        break;
      case 'assembly':
        return new EUtilsAssemblyRepository();
        break;

    }
    return FALSE;
  }

}

