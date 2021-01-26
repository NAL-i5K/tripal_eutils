<?php

/**
 * Maps NCBI Assemblies into a Chado analysis.
 *
 * @ingroup repositories.
 */
class EUtilsAssemblyRepository extends EUtilsRepository {


  /**
   * Chado base table Analysis record.
   *
   * @var array
   */
  protected $base_fields = [
    'name' => '',
    'description' => '',
    'program' => '',
    'programversion' => '',
    'timeexecuted' => '',
    'sourceuri' => NULL,
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
   * Projects linked to this analysis.
   *
   *  This is how biomats will be linked to the analysis.
   *
   * @var array
   */
  protected $projects = [];


  /**
   * Cache of data per run.
   *
   * @var array
   */
  protected static $cache = [
    'db' => [],
    'accessions' => [],
    'analysis' => [],
  ];

  /**
   * Create assembly (chado.analysis) record.
   *
   * @param array $data
   *   The data returned by EUtilsBioProjectParser.
   *
   * @return object
   *   The created bioProject.
   *
   * @throws \Exception
   */
  public function create(array $data) {
    $this->validateFields($data);
    $name = $data['name'];

    $description = $data['description'];

    // Program and program version come from # Assembly method:
    // $data['attributes']['ftp_attributes']['# Assembly method:'].
    $method_string = $data['attributes']['ftp_attributes']['# Assembly method:'];

    // TODO: what do we want to do here?  Parse out the version from the
    // assembly program?  But what if we have multiple programs and
    // versions returned, what then?
    $program = $method_string;
    $programvesion = $method_string;

    $time_executed = $data['submission_date'];
    // Check if we need seconds.
    if ($time_executed && substr_count($time_executed, ':') === 1) {
      $time_executed = $time_executed . ':00';
    }
    $sourcename = $data['sourcename'] ?? NULL;
    // TODO: missing:
    // algorithm, sourceuri, sourceversion, timeexecuted.
    $this->base_fields = [
      'name' => $name,
      'description' => $description,
      'program' => $program,
      'programversion' => $programvesion,
      'sourcename' => $sourcename,
      'timeexecuted' => $time_executed ?? date_now(),
    ];

    $analysis = $this->createAnalysis();

    // Set class base record stuff.
    $this->base_table = 'analysis';
    $this->base_record_id = $analysis->analysis_id;

    // All assemblies are Genome Assembly.  See ticket 197.
    $this->setAnalysisType('Genome Assembly');
    $this->createXMLProp($data['full_ncbi_xml']);

    $this->createAccessions($data['accessions']['assembly']);
    unset($data['accessions']['assembly']);

    $this->createLinkedRecords($data['accessions']);

    $biosamples = $data['accessions']['biosamples'] ?? NULL;

    if ($biosamples && !empty($this->projects)) {
      $biomaterials = $this->getNCBIRecord('biosample', $biosamples);
      $linked = $this->linkBiomaterials($biomaterials);
    }

    // $mapper = new TagMapper();
    // add "stats" as properties.
    foreach ($data['attributes']['stats'] as $key => $value) {

      // TODO: use mapper to look up cvterms.
      // for now just use local.
      // $mapper->lookupAttribute($key)
      $term = tripal_insert_cvterm([
        'id' => 'ncbi_properties:' . $key,
        'name' => $key,
        'def' => '',
        'cv_name' => 'ncbi_properties',
      ]);
      $this->createProperty($term->cvterm_id, $value);
    }

    $this->addFTPLinks($data['attributes']['files']);

    return $analysis;
  }

  /**
   * Creates dbxrefs and linked Chado records.
   *
   * @param array $accessions
   *   Array of other records indexed type => value.
   *
   * @throws \Exception
   */
  public function createLinkedRecords(array $accessions) {
    foreach ($accessions as $accession => $vals) {
      switch ($accession) {
        case 'assembly':
          // Add as a dbxref for this record.
          foreach ($vals as $db => $child) {
            // Multiple assembly accessions.
            if (strpos(strtolower($db), 'assembly')) {
              $db = 'Assembly';
            }
            $this->createAccession(['db' => $db, 'value' => $child]);
          }
          break;

        case 'taxon_accession':
          $organism = $this->getOrganism($vals);
          $linked = $this->linkOrganism($organism);
          break;

        case 'bioprojects':
          $projects = $this->getNCBIRecord('bioproject', $vals);
          $this->projects = $projects;
          $linked = $this->linkProjects($projects);
          break;

        case 'biosamples':
          // Link via project.  We have to do it at the end.
          break;

        default:
          // Generic dbxref of some sort.
          break;
      }
    }
  }

  /**
   * Insert into organism_analysis, or return existing link.
   *
   * @param $organism
   *   Full chado.organism record.
   *
   * @return mixed
   *
   * @throws \Exception
   */
  public function linkOrganism($organism) {
    if (!chado_table_exists('organism_analysis')) {
      throw new Exception("The organism_analysis linker table doesn't exist.  No way to link this organism.");
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

  /**
   * Gets/creates this analysis record.
   */
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

    $cacheKey = implode(':', [$base['name'], $base['program'], $base['programversion']]);
    return static::$cache['analysis'][$cachekey] = $analysis;
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

    // If the analysis is available in our static cache, return it.
    $cacheKey = implode(':', [$base['name'], $base['program'], $base['programversion']]);
    if (isset(static::$cache['analysis'][$cachekey])) {
      return static::$cache['analysis'][$cachekey];
    }

    $exists = db_select('chado.analysis', 't')
      ->fields('t')
      ->condition('name', $base['name'])
      ->condition('program', $base['program'])
      ->condition('programversion', $base['programversion'])
      ->execute()
      ->fetchObject();

    if ($exists) {
      return static::$cache['analysis'][$cacheKey] = $exists;
    }

    return NULL;
  }

  /**
   * Associates FTPs as properties.
   *
   * @param $ftps
   *   Array of key value pars, where the key is the XML FTP type, the value is
   *   the FTP address.
   */
  public function addFTPLinks($ftps) {

    $cvterm_id = chado_get_cvterm(['id' => 'local:ncbi_FTP_links'])->cvterm_id;
    foreach ($ftps as $type => $ftp) {

      $this->createProperty($cvterm_id, $ftp);
    }
  }

  /**
   * Sets the type for the analysis to match bundle.
   *
   * @param string $type
   *   The type/category passed from the parser.
   *
   * @return bool
   */
  private function setAnalysisType(string $type) {

    $term = tripal_get_cvterm(['id' => 'rdfs:type']);

    switch ($type) {
      case 'representative genome':
        return $this->createProperty($term->cvterm_id, 'genome_assembly');

      default:
        return $this->createProperty($term->cvterm_id, 'genome_assembly');
    }
  }

  /**
   * Links biomaterials to the assembly via the associated projects.
   *
   * @param array $biomaterials
   *   Array of biomaterial records.
   *
   * @throws \Exception
   */
  private function linkBiomaterials(array $biomaterials) {

    $projects = $this->projects;

    if (empty($projects)) {
      throw new Exception('No projects associated with this Assembly- cannot associate Biomaterial!');
    }

    foreach ($biomaterials as $biomaterial) {

      foreach ($projects as $project) {

        $values = [
          'biomaterial_id' => $biomaterial->biomaterial_id,
          'project_id' => $project->project_id,
        ];
        // Are this biomaterial and project already linked?  If so continue.
        $exists = chado_generate_var('biomaterial_project', $values);
        if (!$exists) {
          chado_insert_record('biomaterial_project', $values);
        }
      }
    }
  }

  /**
   * Creates a set of accessions (dbxrefs) attached to the analysis.
   *
   * @see \EUtilsRepository::createAccession()
   *
   * @param array $accessions
   *
   * @return array
   */
  private function createAccessions(array $accessions) {
    $data = [];

    foreach ($accessions as $db => $accession) {

      $data[] = $this->createAccession(['db' => $db, 'value' => $accession]);
    }
    return $data;
  }

}
