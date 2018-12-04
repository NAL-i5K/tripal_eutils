<?php

class EUtilsAssemblyRepository extends EUtilsRepository {

  /**
   * Required attributes when using the create method.
   *
   * @var array
   */
  protected $required_fields = [

  ];

  /**
   * Cache of data per run.
   *
   * @var array
   */
  protected static $cache = [
    'db' => [],
    'accessions' => [],
    'analysis',
  ];


  /**
   * Create assembly (chado.analysis) record.
   *
   * @param array $data
   *
   * @return object|void
   */
  public function create($data) {

  }

}