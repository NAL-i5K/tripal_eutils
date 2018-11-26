<?php
namespace Tests;

use StatonLab\TripalTestSuite\DBTransaction;
use StatonLab\TripalTestSuite\TripalTestCase;
module_load_include('inc', 'tripal_eutils', 'includes/Eutils');


class EutilsTest extends TripalTestCase {
  // Uncomment to auto start and rollback db transactions per test method.
  //use DBTransaction;




  /**
   * Basic test example.
   * Tests must begin with the word "test".
   * See https://phpunit.readthedocs.io/en/latest/ for more information.
   */
  public function testBasicExample() {
    $connection = new \Eutils();
    $count = $connection->check_status();
    $this->assertNotFalse($count);
  }

  public function testSettingDB(){
    $connection = new \Eutils();
    $result = $connection->set_db('waffles');

    $this->assertFalse($result);

    $result = $connection->set_db('biosample');
    $this->assertTrue($result);

  }


  public function testBioProjectAttributes(){


    $connection = new \Eutils();
    $connection->set_db('bioproject');
    //https://www.ncbi.nlm.nih.gov/bioproject/PRJNA506315
    $result = $connection->lookup_accessions(['506315']);

    //$result is the XML!

    new xml_parser('bioproject', $result);
    new bioproject_xml_parser($result);


  }
}
