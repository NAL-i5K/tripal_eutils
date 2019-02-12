<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EutilsBioSampleXMLParserTest extends TripalTestCase {

  use DBTransaction;

  /**
   * @test
   * @group biosample
   */
  public function testBioSampleParser() {
    $path = DRUPAL_ROOT . '/' . drupal_get_path('module', 'tripal_eutils');
    foreach (glob("$path/examples/biosamples/*.xml") as $file) {
      $parser = new \EUtilsBioSampleParser();
      $biosample = $parser->parse(simplexml_load_file($file));

      $this->assertArrayHasKey('name', $biosample);
      $this->assertArrayHasKey('accessions', $biosample);
      $this->assertArrayHasKey('attributes', $biosample);
      $this->assertArrayHasKey('description', $biosample);
      $this->assertTrue(is_array($biosample['accessions']));
      $this->assertTrue(is_array($biosample['attributes']));

      $this->assertNotNull($biosample['name']);


      $this->assertNotEmpty($biosample['accessions']);
      $this->assertNotEmpty($biosample['attributes']);

      //Accessions should consist of an aray of arrays.  however hte keys of those arrays seems irregular.

      if ($biosample['name'] == 'SAMN02953603'){
        // This sample has a linked project.  Was it parsed?
        $this->assertArrayHasKey('projects', $biosample);
        $this->assertNotEmpty($biosample['projects']);
      }
    }
  }
}
