<?php

namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;

class EUtilsTest extends TripalTestCase{

  // Uncomment to auto start and rollback db transactions per test method.
  //use DBTransaction;

  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testBasicExample() {
    $connection = new \EUtils();
    $count = $connection->checkStatus('pmc');
    $this->assertNotFalse($count);
  }

  public function testSettingDB() {
    $connection = new \EUtils();
    $this->expectException('Exception');
    $connection->lookupAccessions('waffles', ['000000']);
  }

  public function testBioProjectAttributes() {
    $connection = new \EUtils();
    //$connection->setDB('bioproject');
    //https://www.ncbi.nlm.nih.gov/bioproject/PRJNA506315
    $result = $connection->lookupAccessions('bioproject', ['506315']);

    //$result is the XML!

    //new xml_parser('bioproject', $result);
    //new bioproject_xml_parser($result);
  }
}
