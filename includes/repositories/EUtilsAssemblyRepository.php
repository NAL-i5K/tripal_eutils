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

    //program and program version come from # Assembly method: $data['attributes']['ftp_attributes']['# Assembly method:']

    $method_string = $data['attributes']['ftp_attributes']['# Assembly method:'];

    var_dump($method_string);

    $this->createXMLProp($data['full_ncbi_xml']);

  }



}