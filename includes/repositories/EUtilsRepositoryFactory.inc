<?php

/**
 * Class EUtilsRepositoryFactory.
 *
 * @ingroup repositories
 */
class EUtilsRepositoryFactory implements EUtilsFactoryInterface {

  /**
   * DB to repository mapping.
   *
   * @var array
   */
  protected $repositories = [
    'bioproject' => EUtilsBioProjectRepository::class,
    'biosample' => EUtilsBioSampleRepository::class,
    'assembly' => EUtilsAssemblyRepository::class,
    'pubmed' => EUtilsPubmedRepository::class
  ];

  /**
   * Whether to create linked records.
   *
   * @var bool
   */
  protected $create_linked_records;

  /**
   * EUtilsRepositoryFactory constructor.
   *
   * @param bool $create_linked_records
   *   Whether to create all linked records.
   */
  public function __construct($create_linked_records = TRUE) {
    $this->create_linked_records = $create_linked_records;
  }

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
      throw new Exception('Unable to find a repository for the provided DB: ' . $db);
    }
    return new $this->repositories[$ldb]($this->create_linked_records);
  }

}
