<?php

/**
 * Class EutilsRepositoryFactory.
 */
class EutilsRepositoryFactory implements EUtilsFactoryInterface {

  /**
   * DB to repository mapping.
   *
   * @var array
   */
  protected $repositories = [
    'bioproject' => EUtilsBioProjectRepository::class,
    'biosample' => EUtilsBioSampleRepository::class,
    'assembly' => EUtilsAssemblyRepository::class,
  ];

  /**
   * Get a repository for a given DB.
   *
   * @param string $db
   *   The database name.
   *
   * @return \EUtilsRepository
   *   An initialized instance of the appropriate repository.
   *
   * @throws \Exception
   */
  public function get(string $db) {
    $ldb = strtolower($db);

    if (!isset($this->repositories[$ldb])) {
      throw new Exception('Enable to find a repository for the provided DB: ' . $db);
    }

    return new $this->repositories[$ldb];
  }

}
