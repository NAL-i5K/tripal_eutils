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
   * The fields expected from the XML parser.
   *
   * @var array
   */
  protected $required_fields = [
    'name',
    'description',
    'attributes',
    'full_ncbi_xml',

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
    $this->validateFields($data);
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

    $this->createLinkedRecords($data['accessions']);

    //  $mapper = new TagMapper();

    //add "stats" as properties
    foreach ($data['attributes']['stats'] as $key => $value) {

      //TODO: use mapper to look up cvterms.
      //for now just use local.
      // $mapper->lookupAttribute($key)
      $term = tripal_insert_cvterm([
        'id' => 'ncbi_properties:' . $key,
        'name' => $key,
        'def' => '',
        'cv_name' => 'ncbi_properties',
      ]);
      $this->createProperty($term->cvterm_id, $value);
    }

    //TODO: add FTP links.

    return $analysis;

  }


  /**
   * @param $accessions
   */
  public function createLinkedRecords($accessions) {

    foreach ($accessions as $accession => $vals) {

      switch ($accession) {

        case 'Assembly':
          //add as a dbxref for this record

          break;

        case 'taxon_accession':

          $organism = $this->getOrganism($vals);
          $linked = $this->linkOrganism($organism);

          break;

        case 'bioprojects':
          $projects = $this->getNCBIRecord('bioproject', $vals);
         // $linked = $this->linkProjects($projects);

          break;

        case 'biosamples':
         // $biomaterials = $this->getBiomaterials($vals);
         // $linked = $this->linkBiomaterials($biomaterials);

          break;

        default:
          //generic dbxref of some sort.
          break;


      }

    }


  }




    /**
   * Insert into organism_analysis, or return existing link.
   *
   * @param $organism
   * Full chado.organism record.
   *
   * @return mixed
   * @throws \Exception
   */
  public function linkOrganism($organism) {

    if (!chado_table_exists('organism_analysis')) {
      throw new Exception('The organism_analysis linker table doesnt exist.  No way to link this organism.');
    }

    $result = db_select('chado.organism_analysis', 't')
      ->fields('t', ['organism_analysis_id'])
      ->condition('t.organism_id', $organism->organism_id)
      ->condition('t.analysis_id', $this->base_record_id)
      ->execute()
      ->fetchField();

    if (!$result) {

      $result = db_insert('chado.organism_analysis')
        ->fields([
          'organism_id' => $organism->organism_id,
          'analysis_id' => $this->base_record_id,
        ])
        ->execute();
    }

    if (!$result) {
      throw new Exception('Could not link organism to analysis.');
    }

    return $result;
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
