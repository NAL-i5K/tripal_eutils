<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsAssemblyRepositoryTest extends TripalTestCase {

  // Uncomment to auto start and rollback db transactions per test method.
   use DBTransaction;


  /**
   * @group analysis
   * @group assembly
   * @group repository
   * @group wip
   */
  public function testAssemblyFromXML() {


    $file = __DIR__ . '/../examples/assembly/559011_assembly.xml';

    $parser = new \EUtilsAssemblyParser();
    $assembly = $parser->parse(simplexml_load_file($file));

    $repo = new \EUtilsAssemblyRepository();

    // Make sure creating a new one works
    $name = 'wgs.5d';
    $result = $repo->create($assembly);

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
}
