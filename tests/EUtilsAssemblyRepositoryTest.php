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
   * @group wip
   */
  public function testAssemblyFromXML() {

    $result = $this->parseAndCreateAsembly();

    $this->assertObjectHasAttribute('analysis_id', $result);


    $props = db_select('chado.analysisprop', 't')
      ->fields('t')
      ->condition('t.analysis_id', $result->analysis_id)
      ->execute()
      ->fetchAll();

    //at a minimum we have the raw XML prop.
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
  }

  public function testAssemblyCreatesOrganism() {

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


  private function parseAndCreateAsembly() {

    //Oops, this doesnt work like i expect it to, this code never fires....
    if ($this->repository != NULL) {
      return $this->repository;
    }

    $file = __DIR__ . '/../examples/assembly/559011_assembly.xml';


    $parser = $this->getMockBuilder('\EUtilsAssemblyParser')
      ->setMethods(['getFTPData'])
      ->getMock();

    //We mock the FTP call.
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
}
