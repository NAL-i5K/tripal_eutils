<?php

class EUtilsAssemblyRepository extends EUtilsRepository {


  /**
   * chado base table Analysis record.
   *
   * @var array
   */
  protected $base_fields = [
    'name' => '',
    'description' => '',
    'program' => '',
    'programversion' => '',
    'timeexecuted' => '',
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

    $name = $data['name'];

    $description = $data['description'];

    //program and program version come from # Assembly method: $data['attributes']['ftp_attributes']['# Assembly method:']

    $method_string = $data['attributes']['ftp_attributes']['# Assembly method:'];

    //TODO: what do we want to do here?  Parse out the verion from the assembly program?  But what if we have multiple programs and versions returned, what then?
    $program = $method_string;
    $programvesion = $method_string;


    //TODO: missing:
    //algorithm, sourcename, sourceversion, sourceuri, timeexecuted.



    $this->base_fields = [
      'name' => $name,
      'description' => $description,
      'program' => $program,
      'programversion' => $programvesion,
      'timeexecuted' => date_now(),
    ];


    $analysis = $this->createAnalysis();

    //Set class base record stuff
    $this->base_table = 'analysis';
    $this->base_record_id = $analysis->analysis_id;


    //Set the type of the analysis so it maps to a bundle.
    //consider: genome assembly, transcriptome, etc.
    //TODO: do we let users map which analysis to create?
    $term = tripal_get_cvterm(['id' => 'rdfs:type']);
    $this->createProperty($term->cvterm_id, 'genome_assembly');
    $this->createXMLProp($data['full_ncbi_xml']);

    return $analysis;

  }


  public function createAnalysis() {
    // Name is unique so find project.
    $record = $this->getAnalysis();

    if (!empty($record)) {
      return $record;
    }

    $base = $this->base_fields;

    $id = db_insert('chado.analysis')->fields([
      'name' => $base['name'],
      'description' => $base['description'] ?? '',
      'program' => $base['program'],
      'programversion' => $base['programversion'],
      'algorithm' => $base['algorithm'] ?? '',
      'sourcename' => $base['sourcename'] ?? '',
      'sourceversion' => $base['sourceversion'] ?? '',
      'sourceuri' => $base['sourceuri'] ?? '',
      'timeexecuted' => $base['timeexecuted'] ?? date_now(),
    ])->execute();

    if (!$id) {
      throw new Exception('Unable to create chado.analysis record');
    }

    $analysis = db_select('chado.analysis', 't')
      ->fields('t')
      ->condition('analysis_id', $id)
      ->execute()
      ->fetchObject();

    return static::$cache['analysis'] = $analysis;
  }

  /**
   * Get analysis from db or cache.
   *
   * @param string $name
   *
   * @return null
   */
  public function getAnalysis() {

    $base = $this->base_fields;
    // If the project is available in our static cache, return it
    if (isset(static::$cache['analysis'])) {
      return static::$cache['analysis'];
    }


    $exists = db_select('chado.analysis', 't')
      ->fields('t')
      ->condition('name', $base['name'])
      ->condition('program', $base['program'])
      ->condition('programversion', $base['programversion'])
      ->execute()
      ->fetchObject();

    if ($exists) {

      //TODO: we need to carefully validate the returned analysis.  We want to be sure we arent overwriting another existing analysis....

      return static::$cache['analysis'] = $exists;
    }

    return NULL;
  }

}