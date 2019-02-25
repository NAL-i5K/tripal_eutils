<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsAssemblyRepositoryTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
  use DBTransaction;


  /**
   * Holds the repository that has been handed the parsed XML.
   */
  private $repository = NULL;

  /**
   * @group analysis
   * @group assembly
   * @group repository
   */
  public function testAssemblyFromXML() {

    $result = $this->parseAndCreateAsembly();

    $this->assertObjectHasAttribute('analysis_id', $result);

    $this->assertEquals('wgs.5d', $result->name);

    $this->assertEquals('2015-10-22 00:00:00', $result->timeexecuted);

    $props = db_select('chado.analysisprop', 't')
      ->fields('t')
      ->condition('t.analysis_id', $result->analysis_id)
      ->execute()
      ->fetchAll();

    $this->assertNotEmpty($props);


    $xml_term = tripal_get_cvterm(['id' => 'local:full_ncbi_xml']);


    $props = db_select('chado.analysisprop', 't')
      ->fields('t')
      ->condition('t.analysis_id', $result->analysis_id)
      ->condition('t.type_id', $xml_term->cvterm_id)
      ->execute()
      ->fetchObject();

    //at a minimum we have the raw XML prop.
    $this->assertNotFalse($props);


    $ftp_term = chado_get_cvterm(['id' => 'local:ncbi_FTP_links']);

    $props = db_select('chado.analysisprop', 't')
      ->fields('t', ['type_id'])
      ->condition('t.analysis_id', $result->analysis_id)
      ->condition('t.type_id', $ftp_term->cvterm_id)
      ->execute()
      ->fetchAll();

    $this->assertNotEmpty($props);

    $type_term = chado_get_cvterm(['id' => 'rdfs:type']);
    $type = db_select('chado.analysisprop', 't')
      ->fields('t', ['value'])
      ->condition('t.analysis_id', $result->analysis_id)
      ->condition('t.type_id', $type_term->cvterm_id)
      ->execute()
      ->fetchObject();
    $this->assertEquals('genome_assembly', $type->value, 'Analysis type for assembly was not genome_assembly');

  }

  public function testAssemblyLinksOrganism() {

    $repo = new \EUtilsAssemblyRepository();
    $analysis = factory('chado.analysis')->create();
    $organism = factory('chado.organism')->create();
    $repo->setBaseRecordId($analysis->analysis_id);

    $repo->linkOrganism($organism);

    $result = db_select('chado.organism_analysis', 't')
      ->fields('t', ['organism_analysis_id'])
      ->condition('t.organism_id', $organism->organism_id)
      ->condition('t.analysis_id', $analysis->analysis_id)
      ->execute()
      ->fetchField();

    $this->assertNotFalse($result);
  }

  /**
   * @group organism
   * @group assembly
   */
  public function testAssemblyCreatesOrganismFromNCBIAccession() {


    $accessions = ['taxon_accession' => '499546'];

    $repo = new \EUtilsAssemblyRepository();
    $analysis = factory('chado.analysis')->create();

    $repo->setBaseRecordId($analysis->analysis_id);
    $repo->createLinkedRecords($accessions);

    $organism = db_select('chado.organism', 't')
      ->fields('t')
      ->condition('t.genus', 'Tamias')
      ->execute()
      ->fetchObject();

    $this->assertNotFalse($organism, 'NCBI accession 499546 not created via AssemblyRepo createLinkedRecords');


    $result = db_select('chado.organism_analysis', 't')
      ->fields('t', ['organism_analysis_id'])
      ->condition('t.organism_id', $organism->organism_id)
      ->condition('t.analysis_id', $analysis->analysis_id)
      ->execute()
      ->fetchField();

    $this->assertNotFalse($result, 'organism was not linked to analysis via createLinkedRecords');


  }

  public function testAssemblyLinksExistingOrganism() {


    $accessions = ['taxon_accession' => '499546'];

    $repo = new \EUtilsAssemblyRepository();
    $analysis = factory('chado.analysis')->create();
    $organism_original = factory('chado.organism')->create([
      'genus' => 'Tamias',
      'species' => 'alpinus',
      'abbreviation' => 'T. alpinus',
      'common_name' => 'waffle_monster',
    ]);

    $repo->setBaseRecordId($analysis->analysis_id);
    $repo->createLinkedRecords($accessions);

    $organism = db_select('chado.organism', 't')
      ->fields('t')
      ->condition('t.genus', 'Tamias')
      ->execute()
      ->fetchObject();

    $this->assertNotFalse($organism, 'NCBI accession 499546 not created via AssemblyRepo createLinkedRecords');
    $this->assertEquals('waffle_monster', $organism->common_name, 'NCBI accession 499546 overwrote existing common name value');


    $result = db_select('chado.organism_analysis', 't')
      ->fields('t', ['organism_analysis_id'])
      ->condition('t.organism_id', $organism->organism_id)
      ->condition('t.analysis_id', $analysis->analysis_id)
      ->execute()
      ->fetchField();

    $this->assertNotFalse($result, 'organism was not linked to analysis via createLinkedRecords');

    $this->assertEquals($organism_original->organism_id, $organism->organism_id);

  }


  /**
   * @group project_linker
   * @throws \Exception
   */
  public function testAssemblyLinksProject() {

    $repo = new \EUtilsAssemblyRepository();
    $analysis = factory('chado.analysis')->create();
    $project = factory('chado.project')->create();
    $repo->setBaseRecordId($analysis->analysis_id);
    $repo->setBaseTable('analysis');

    $repo->linkProjects([$project]);

    $result = db_select('chado.project_analysis', 't')
      ->fields('t', ['project_analysis_id'])
      ->condition('t.project_id', $project->project_id)
      ->condition('t.analysis_id', $analysis->analysis_id)
      ->execute()
      ->fetchField();

    $this->assertNotFalse($result);
  }

  //
  //  /**
  //   * Note this test uses the network connection to pull the referenced
  //   * bioproject.  It succeeds if run alone but fails with all others.
  //   *
  //   * @group network
  //   * @throws \Exception
  //   */
  //  public function testAssemblyCreatesProjectFromNCBIAccession() {
  //
  //    //provide actual accession
  //    $accessions = ['bioprojects' => ['291087']];
  //
  //    $repo = new \EUtilsAssemblyRepository();
  //    $analysis = factory('chado.analysis')->create();
  //
  //    //TODO:  provide a fake EUtils populated iwth the project.
  //
  //    $repo->setBaseRecordId($analysis->analysis_id);
  //    $repo->setBaseTable('analysis');
  //
  //    $repo->createLinkedRecords($accessions);
  //
  //    $result = db_select('chado.project_analysis', 't')
  //      ->fields('t', ['project_id'])
  //      ->condition('t.analysis_id', $analysis->analysis_id)
  //      ->execute()
  //      ->fetchField();
  //
  //    $this->assertNotFalse($result, 'No projects were linked to the analysis!');
  //
  //    $project = db_select('chado.project', 't')
  //      ->fields('t')
  //      ->condition('project_id', $result)
  //      ->execute()
  //      ->fetchObject();
  //
  //    $this->assertNotFalse($project);
  //  }

  private function parseAndCreateAsembly() {

    // Oops, this doesnt work like i expect it to, this code never fires....
    if ($this->repository != NULL) {
      return $this->repository;
    }

    $file = __DIR__ . '/../../examples/assembly/559011_assembly.xml';


    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();

    // We mock the FTP call.
    $ftp_response = ['# Assembly method:' => 'a method, v1.0'];

    $parser->expects($this->once())
      ->method('getFTPData')
      ->will($this->returnValue($ftp_response));

    $assembly = $parser->parse(simplexml_load_file($file));

    $repo = new \EUtilsAssemblyRepository();

    $result = $repo->create($assembly);

    $this->repository = $result;

    return $result;
  }


  /**
   * @group assembly
   * @group failing
   * @throws \Exception
   */
  public function testLinkingBiomatsViaProject() {

    $projectA = factory('chado.project')->create();
    $projectB = factory('chado.project')->create();
    $analysis = factory('chado.analysis')->create();
    $biomatA = factory('chado.biomaterial')->create();
    $biomatB = factory('chado.biomaterial')->create();



    $repo_parent = new \EUtilsAssemblyRepository();
    $repo = reflect($repo_parent);
    $repo->setBaseRecordId($analysis->analysis_id);
    $repo->projects = [$projectA, $projectB];
    $repo->linkBiomaterials([$biomatA, $biomatB]);

    $a = db_select('chado.biomaterial_project', 't')
      ->fields('t')
      ->condition('t.project_id', $projectA->project_id)
      ->execute()
      ->fetchAll();

    $this->assertNotFalse($a);
    $this->assertEquals(2, count($a), 'Project A not linked to both biomaterials');


    $b = db_select('chado.biomaterial_project', 't')
      ->fields('t')
      ->condition('t.project_id', $projectB->project_id)
      ->execute()
      ->fetchAll();

    $this->assertNotFalse($b);
    $this->assertEquals(2, count($b), 'Project B not linked to both biomaterials');


  }

}
